#include <stdio.h>
#include <stdlib.h>
#include "emboss.h"

#include "soapH.h"
#include "GLANGSoapBinding.nsmap"

#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"

int main(int argc, char *argv[]){
  embInitPV("gseqinfo",argc,argv,"GEMBASSY","0.0.1");
  
  struct soap soap;
  
  AjPSeq    seq   = NULL;
  AjPStr    inseq = NULL;
  int i;
  char*     jobid;

  seq = ajAcdGetSeq("sequence");
  
    
  soap_init(&soap);
  
  inseq = NULL;
    ajStrAppendC(&inseq,">");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
    ajStrAppendC(&inseq,"\n");
    ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
  
  char* in0;
  in0 = ajCharNewS(inseq);
  if(soap_call_ns1__seqinfo(&soap,NULL,NULL,in0,&jobid)==SOAP_OK){
    char* dlm = "<>";
    char* tp;
    int   list[4];
    tp = strtok(jobid,dlm);
    i=1;
    while(tp != NULL){
      tp = strtok(NULL,dlm);
      if(i%3==1){list[(i-1)/3]=atoi(tp);}
      i++;
    }
    printf("A\tT\tG\tC\t\n%d\t%d\t%d\t%d\t\n",list[0],list[1],list[2],list[3]);
  }else{
    soap_print_fault(&soap,stderr);
  }
  
  soap_destroy(&soap);
  soap_end(&soap);
  soap_done(&soap);
  
  embExit();
  return 0;
}