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
  embInitPV("ggcskew",argc,argv,"GEMBASSY","1.0.0");

  struct soap soap;
  struct ns1__gcskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  ajint     window     = 0;
  ajint     slide      = 0;
  AjBool    cumulative = 0;
  AjBool    at         = 0;
  AjBool    purine     = 0;
  AjBool    keto       = 0;
  AjPStr    output     = NULL;
  AjPStr    accid      = NULL;
  AjPStr    filename   = NULL;
  char*     _result; 
  char*     jobid;

  AjPGraph    mult;
  gPlotParams gpp;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  slide      = ajAcdGetInt("slide");
  cumulative = ajAcdGetBoolean("cumulative");
  at         = ajAcdGetBoolean("at");
  purine     = ajAcdGetBoolean("purine");
  keto       = ajAcdGetBoolean("keto");
  output     = ajAcdGetString("output");
  accid      = ajAcdGetString("accid");
  mult       = ajAcdGetGraphxy("graph");

  params.window       = window;
  params.slide        = slide;
  if(cumulative){
    params.cumulative = 1;
  }else{
    params.cumulative = 0;
  }
  if(at){
    params.at         = 1;
  }else{
    params.at         = 0;
  }
  if(purine){
    params.purine     = 1;
  }else{
    params.purine     = 0;
  }
  if(keto){
    params.keto       = 1;
  }else{
    params.keto       = 0;
  }
  params.output       = "f";
  
  while(ajSeqallNext(seqall,&seq)){

    soap_init(&soap);

    inseq = NULL;

    ajStrAppendC(&inseq,">");
    ajStrAppendS(&inseq,ajSeqGetNameS(seq));
    ajStrAppendC(&inseq,"\n");
    ajStrAppendS(&inseq,ajSeqGetSeqS(seq));
    ajStrAssignS(&accid,ajSeqGetAccS(seq));

    char* in0;
    in0 = ajCharNewS(inseq);

    fprintf(stderr,"%s\n",ajCharNewS(ajSeqGetAccS(seq)));

    if(soap_call_ns1__gcskew(&soap,NULL,NULL,in0,&params,&jobid)==SOAP_OK){
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
