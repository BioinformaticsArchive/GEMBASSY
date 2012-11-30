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
	embInitPV("gfind_ori_ter", argc, argv, "GEMBASSY", "1.0.0");

	struct soap soap;
	struct ns1__find_USCOREori_USCOREterInputParams params;

	AjPSeqall seqall;
	AjPSeq    seq;
	AjPStr    inseq    = NULL;
	ajint     window   = 0;
	AjBool    purine   = 0;
	AjBool    keto     = 0;
	ajint     filter   = 0;
	AjPStr    accid    = NULL;
	char*     result;

	AjBool  show = 0;
	AjPFile outf = NULL;

	seqall = ajAcdGetSeqall("sequence");
	window = ajAcdGetInt("window");
	filter = ajAcdGetInt("filt");
	purine = ajAcdGetBoolean("purine");
	keto   = ajAcdGetBoolean("keto");
	accid  = ajAcdGetString("accid");

	show = ajAcdGetToggle("show");
	outf = ajAcdGetOutfile("outfile");

	params.window   = window;
	params.filter   = filter;
	if(purine){
		params.purine = 1;
	}else{
		params.purine = 0;
	}
	if(keto){
		params.keto   = 1;
	}else{
		params.keto   = 0;
	}

	while(ajSeqallNext(seqall, &seq)){

		soap_init(&soap);

		inseq = NULL;

		if(ajSeqGetFeat(seq) && !ajStrGetLen(accid)){
			inseq = getGenbank(seq);
			ajStrAssignS(&accid, ajSeqGetAccS(seq));
		}else{
			if(!ajStrGetLen(accid)){
				fprintf(stderr, "Sequence does not have features\n");
				fprintf(stderr, "Proceeding with sequence accession ID\n");
				ajStrAssignS(&accid,ajSeqGetAccS(seq));
			}
			if(!valID(ajCharNewS(accid))){
				fprintf(stderr, "Invalid accession ID, exiting");
				return 1;
			}
			ajStrAssignS(&inseq, accid);
		}

		char* in0;
		in0 = ajCharNewS(inseq);

		if(soap_call_ns1__find_USCOREori_USCOREter(
					&soap, NULL, NULL,
					in0, &params, &result
					) == SOAP_OK){
			AjPStr tmp = ajStrNewC(result);
			AjPStr parse = ajStrNew();
			AjPStr ori = NULL;
			AjPStr ter = NULL;
			AjPStrTok handle = NULL;
			ajStrExchangeCC(&tmp, "<", "\n");
			ajStrExchangeCC(&tmp, ">", "\n");
			handle = ajStrTokenNewC(tmp, "\n");
			while(ajStrTokenNextParse(&handle, &parse)){
				if(ajStrIsInt(parse))
					if(!ori)
						ori = ajStrNewS(parse);
					else if(!ter)
						ter = ajStrNewS(parse);
			}
			if(show)
				ajFmtPrint("Sequence: %S Origin: %S Terminus: %S\n",
						accid, ori, ter);
			else
				ajFmtPrintF(outf, "Sequence: %S Origin: %S Terminus: %S\n",
						accid, ori, ter);
		}else{
			soap_print_fault(&soap, stderr);
		}

		soap_destroy(&soap);
		soap_end(&soap);
		soap_done(&soap);
	}

	if(outf)
		ajFileClose(&outf);

	ajSeqallDel(&seqall);
	ajSeqDel(&seq);
	ajStrDel(&inseq);

	embExit();
	return 0;
}
