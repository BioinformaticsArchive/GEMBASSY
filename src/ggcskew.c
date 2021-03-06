/******************************************************************************
** @source ggcskew
**
** Calculates the GC skew of the input sequence
**
** @author Copyright (C) 2012 Hidetoshi Itaya
** @version 1.0.1   Revision 1
** @modified 2012/1/20  Hidetoshi Itaya  Created!
** @modified 2013/6/16  Revision 1
** @@
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
******************************************************************************/

#include "emboss.h"
#include "soapH.h"
#include "GLANGSoapBinding.nsmap"
#include "soapClient.c"
#include "soapC.c"
#include "../gsoap/stdsoap2.c"
#include "glibs.h"




/* @prog ggcskew **************************************************************
**
** Calculates the GC skew of the input sequence
**
******************************************************************************/

int main(int argc, char *argv[])
{
  embInitPV("ggcskew", argc, argv, "GEMBASSY", "1.0.1");

  struct soap soap;
  struct ns1__gcskewInputParams params;

  AjPSeqall seqall;
  AjPSeq    seq;
  AjPStr    inseq      = NULL;
  AjPStr    seqid      = NULL;
  ajint	    window     = 0;
  ajint	    slide      = 0;
  AjBool    cumulative = 0;
  AjBool    at         = 0;
  AjBool    purine     = 0;
  AjBool    keto       = 0;

  char *in0;
  char *result;

  AjBool      plot = 0;
  AjPFile     outf = NULL;
  AjPFilebuff buff = NULL;
  AjPGraph    mult = NULL;

  gPlotParams gpp;
  AjPStr      title = NULL;

  seqall     = ajAcdGetSeqall("sequence");
  window     = ajAcdGetInt("window");
  slide      = ajAcdGetInt("slide");
  cumulative = ajAcdGetBoolean("cumulative");
  at         = ajAcdGetBoolean("at");
  purine     = ajAcdGetBoolean("purine");
  keto       = ajAcdGetBoolean("keto");

  plot = ajAcdGetToggle("plot");
  outf = ajAcdGetOutfile("outfile");
  mult = ajAcdGetGraphxy("graph");

  params.window     = window;
  params.slide      = slide;
  params.cumulative = 0;
  params.at         = 0;
  params.purine     = 0;
  params.keto       = 0;
  params.output     = "f";

  if(cumulative)
    params.cumulative = 1;
  if(at)
    params.at = 1;
  if(purine)
    params.purine = 1;
  if(keto)
    params.keto = 1;

  while(ajSeqallNext(seqall, &seq))
    {
      soap_init(&soap);

      inseq = NULL;

      ajStrAppendC(&inseq, ">");
      ajStrAppendS(&inseq, ajSeqGetNameS(seq));
      ajStrAppendC(&inseq, "\n");
      ajStrAppendS(&inseq, ajSeqGetSeqS(seq));

      ajStrAssignS(&seqid, ajSeqGetAccS(seq));

      in0 = ajCharNewS(inseq);

      if(soap_call_ns1__gcskew(
			      &soap,
			       NULL,
			       NULL,
			       in0,
			      &params,
			      &result
			      ) == SOAP_OK)
	{
	  if(plot)
	    {
	      title = ajStrNew();

	      ajStrAppendC(&title, argv[0]);
	      ajStrAppendC(&title, " of ");
	      ajStrAppendS(&title, seqid);

	      gpp.title = ajStrNewS(title);
	      gpp.xlab = ajStrNewC("location");
	      gpp.ylab = ajStrNewC("GC skew");

	      if(!gFilebuffURLC(result, &buff))
		{
                  ajDie("File downloading error from:\n%s\n", result);
		}

	      if(!gPlotFilebuff(buff, mult, &gpp))
		{
		  ajDie("Error in plotting\n");
		}

	      AJFREE(gpp.title);
	      AJFREE(gpp.xlab);
	      AJFREE(gpp.ylab);
	      ajStrDel(&title);
              ajFilebuffDel(&buff);
	    }
	  else
	    {
	      ajFmtPrintF(outf, "Sequence: %S\n", seqid);
	      if(!gFileOutURLC(result, &outf))
		{
                  ajDie("File downloading error from:\n%s\n", result);
		}
	    }
	}
      else
	{
          soap_print_fault(&soap, stderr);
        }

    soap_destroy(&soap);
    soap_end(&soap);
    soap_done(&soap);

    AJFREE(in0);

    ajStrDel(&inseq);
  }

  ajFileClose(&outf);

  ajSeqallDel(&seqall);
  ajSeqDel(&seq);
  ajStrDel(&seqid);

  embExit();

  return 0;
}
