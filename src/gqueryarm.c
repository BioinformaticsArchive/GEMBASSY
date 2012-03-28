#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInit("gqueryarm",argc,argv);

  struct soap soap;

  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     position;
  char*     jobid;

  seq        = ajAcdGetSeq("sequence");
  position   = ajAcdGetInt("position");

  soap_init(&soap);
  
  inseq = NULL;
    seq=ajAcdGetSeq("sequence");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__query_USCOREarm(&soap,NULL,NULL,in0,position,&jobid)==SOAP_OK){
    printf("%s\n",jobid);
  }else{
    soap_print_fault(&soap,stderr);
  }
      
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);

  embExit();
  return 0;
}