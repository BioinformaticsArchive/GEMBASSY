#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"
#include "../include/gplot.h"

int main(int argc, char *argv[]){
  embInitPV("gview_cds",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__view_USCOREcdsInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq    = NULL;
  ajint     length   = 0;
  ajint     gap      = 0;
  AjPStr    accid    = NULL;
  AjPStr    filename = NULL;
  char*     jobid;

  AjPGraph    mult;
  gPlotParams gpp;

  seqall = ajAcdGetSeqall("sequence");
  length = ajAcdGetInt("length");
  gap    = ajAcdGetInt("gap");
  accid  = ajAcdGetString("accid");
  mult   = ajAcdGetGraphxy("graph");
  
  params.length = length;
  params.gap    = gap;
  params.output = "f";

  while(ajSeqallNext(seqall,&seq)){  

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !strlen(ajCharNewS(accid))){
      inseq = getGenbank(seq,ajSeqGetFeat(seq));
      ajStrAssignS(&accid,ajSeqGetAccS(seq));
    }else{
      if(!strlen(ajCharNewS(accid))){
        fprintf(stderr,"Sequence does not have features\n");
        fprintf(stderr,"Proceeding with sequence accession ID\n");
        ajStrAssignS(&inseq,ajSeqGetAccS(seq));
        ajStrAssignS(&accid,ajSeqGetAccS(seq));
      }
      if(!valID(ajCharNewS(accid))){
          fprintf(stderr,"Invalid accession ID, exiting");
          return 1;
      }else{
        ajStrAssignS(&inseq,accid);
      }
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));

    if(soap_call_ns1__view_USCOREcds(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      ajStrAppendC(&filename,".csv");
      if(get_file(jobid,ajCharNewS(filename))==0){
        AjPStr title = NULL;
        ajStrAppendC(&title, argv[0]);
        ajStrAppendC(&title, " of ");
        ajStrAppendS(&title, accid);
        gpp.title = ajStrNewS(title);
        gpp.xlab = ajStrNewC("location");
        gpp.ylab = ajStrNewC("GC skew");
        ajStrDel(&title);
        if(gPlotFile(filename, mult, &gpp) == 1)
          fprintf(stderr,"Error allocating\n");
       }else{
        fprintf(stderr,"Retrieval unsuccessful\n");
      }
    }else{
      soap_print_fault(&soap,stderr);
    }
    
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&inseq);
  ajStrDel(&filename);
    
  embExit();
  return 0;
}
