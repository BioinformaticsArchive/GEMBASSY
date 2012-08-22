#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "../include/getfile.h"

int main(int argc, char *argv[]){
  embInitPV("gdelta_enc",argc,argv,"GEMBASSY","1.0.0");
  
  struct soap soap;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq  = NULL;
  AjPStr    filename   = NULL;
  char*     jobid;

  seqall = ajAcdGetSeqall("sequence");
  
  while(ajSeqallNext(seqall,&seq)){
    soap_init(&soap);

    inseq = NULL;
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
    
    char* in0;
    in0 = ajCharNewS(inseq);
    if(soap_call_ns1__delta_USCOREenc(&soap,NULL,NULL,in0,&jobid)==SOAP_OK){
      puts(jobid);
    }else{
      soap_print_fault(&soap,stderr);
    }
  
    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);
  }
  
  embExit();
  return 0;
}
