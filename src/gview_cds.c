#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/gembassy.h"

int main(int argc, char *argv[]){
  embInitPV("gview_cds",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__view_USCOREcdsInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq     = NULL;
  ajint     length    = 0;
  ajint     gap       = 0;
  AjPStr    output    = NULL;
  AjBool    accid     = 0;
  AjPStr    filename  = NULL;
  char*     jobid;

  seqall = ajAcdGetSeqall("sequence");
  length = ajAcdGetInt("length");
  gap    = ajAcdGetInt("gap");
  output = ajAcdGetString("output");
  accid  = ajAcdGetBoolean("accid");
  
  params.length = length;
  params.gap    = gap;
  params.output = ajCharNewS(output);

  while(ajSeqallNext(seqall,&seq)){  

    soap_init(&soap);

    inseq = NULL;

    if(ajSeqGetFeat(seq) && !accid){
      inseq = getGenbank(seq);
    }else{
      ajStrAppendS(&inseq,ajSeqGetAccS(seq));
    }

    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));

    if(!ajSeqGetFeat(seq) && !accid)
      fprintf(stderr,"Sequence does not have features\nProceeding with sequence accession ID\n");

    if(soap_call_ns1__view_USCOREcds(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
      ajStrAssignS(&filename,ajSeqGetNameS(seq));
      if(strcmp(params.output,"g") == 0){
        ajStrAppendC(&filename,".png");
      }else{
        ajStrAppendC(&filename,".csv");
      }
      if(get_file(jobid,ajCharNewS(filename))==0){
        fprintf(stderr,"Retrieval successful\n");
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
