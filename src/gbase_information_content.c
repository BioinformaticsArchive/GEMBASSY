#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"
#include "../include/display_png.h"
#include "../include/gplot.h"

int main(int argc, char *argv[]){
  embInitPV("gbase_information_content",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__base_USCOREinformation_USCOREcontentInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    position   = NULL;
  ajint     PatLen     = 0;
  ajint     upstream   = 0;
  ajint     downstream = 0;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     jobid; 

  AjPGraph    mult;
  gPlotParams gpp;

  seqall     = ajAcdGetSeqall("sequence");
  position   = ajAcdGetString("position");
  PatLen     = ajAcdGetInt("patlen");
  upstream   = ajAcdGetInt("upstream");
  downstream = ajAcdGetInt("downstream");
  accid      = ajAcdGetString("accid");
  mult       = ajAcdGetGraphxy("graph");

  params.position   = ajCharNewS(position);
  params.PatLength  = PatLen;
  params.upstream   = upstream;
  params.downstream = downstream;
  params.output     = "f";
  
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

    if(soap_call_ns1__base_USCOREinformation_USCOREcontent(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      ajStrAppendC(&filename,".csv");
      if(get_file(jobid,ajCharNewS(filename))==0){
	AjPStr title = NULL;
        ajStrAppendC(&title, argv[0]);
        ajStrAppendC(&title, " of ");
        ajStrAppendS(&title, accid);
        gpp.title = ajStrNewS(title);
        gpp.xlab = ajStrNewC("position");
        gpp.ylab = ajStrNewC("information content");
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
