/*
 *  TEST SUITE FOR MB/WC FUNCTIONS IN C LIBRARY
 *
 *	 FILE:	dat_iswalpha.c
 *
 *	 ISW*:	int iswalpha (wint_t wc);
 */


#include "dat_isw-funcs.h"


TST_ISW_LOC(ALPHA, alpha)  = {

	{
		TST_ISW_REC(de, alpha)
		{
			{  { 0x0080 }, { 0, 1, 0 }
			},  /* CTRL     */

			{  { 0x009F }, { 0, 1, 0 }  }, /* CTRL     */
			{  { 0x00A0 }, { 0, 1, 0 }  }, /* NB SPACE */
			{  { 0x00A1 }, { 0, 1, 0 }  }, /* UD !     */
			{  { 0x00B0 }, { 0, 1, 0 }  }, /* Degree   */
			{  { 0x00B1 }, { 0, 1, 0 }  }, /* +- sign  */
			{  { 0x00B2 }, { 0, 1, 0 }  }, /* SUP 2    */
			{  { 0x00B3 }, { 0, 1, 0 }  }, /* SUP 3    */
			{  { 0x00B4 }, { 0, 1, 0 }  }, /* ACUTE    */
			{  { 0x00B8 }, { 0, 1, 0 }  }, /* CEDILLA  */
			{  { 0x00B9 }, { 0, 1, 0 }  }, /* SUP 1    */
			{  { 0x00BB }, { 0, 1, 0 }  }, /* >>	      */
			{  { 0x00BC }, { 0, 1, 0 }  }, /* 1/4      */
			{  { 0x00BD }, { 0, 1, 0 }  }, /* 1/2      */
			{  { 0x00BE }, { 0, 1, 0 }  }, /* 3/4      */
			{  { 0x00BF }, { 0, 1, 0 }  }, /* UD ?     */
			{  { 0x00C0 }, { 0, 0, 0 }  }, /* A Grave  */
			{  { 0x00D6 }, { 0, 0, 0 }  }, /* O dia    */
			{  { 0x00D7 }, { 0, 1, 0 }  }, /* multipl. */
			{  { 0x00D8 }, { 0, 0, 0 }  }, /* O stroke */
			{  { 0x00DF }, { 0, 0, 0 }  }, /* small Sh */
			{  { 0x00E0 }, { 0, 0, 0 }  }, /* a grave  */
			{  { 0x00F6 }, { 0, 0, 0 }  }, /* o dia    */
			{  { 0x00F7 }, { 0, 1, 0 }  }, /* division */
			{  { 0x00F8 }, { 0, 0, 0 }  }, /* o stroke */
			{  { 0x00FF }, { 0, 0, 0 }  }, /* y dia    */
			{  is_last: 1 }		  /* last element    */
		}
	},
	{
		TST_ISW_REC(enUS, alpha)
		{
			{  { WEOF   }, { 0, 1, 0 }
			},

			{  { 0x0000 }, { 0, 1, 0 }  },
			{  { 0x001F }, { 0, 1, 0 }  },
			{  { 0x0020 }, { 0, 1, 0 }  },
			{  { 0x0021 }, { 0, 1, 0 }  },
			{  { 0x002F }, { 0, 1, 0 }  },
			{  { 0x0030 }, { 0, 1, 0 }  },
			{  { 0x0039 }, { 0, 1, 0 }  },
			{  { 0x003A }, { 0, 1, 0 }  },
			{  { 0x0040 }, { 0, 1, 0 }  },
			{  { 0x0041 }, { 0, 0, 0 }  },
			{  { 0x005A }, { 0, 0, 0 }  },
			{  { 0x005B }, { 0, 1, 0 }  },
			{  { 0x0060 }, { 0, 1, 0 }  },
			{  { 0x0061 }, { 0, 0, 0 }  },
			{  { 0x007A }, { 0, 0, 0 }  },
			{  { 0x007B }, { 0, 1, 0 }  },
			{  { 0x007E }, { 0, 1, 0 }  },
			{  { 0x007F }, { 0, 1, 0 }  },
			{  { 0x0080 }, { 0, 1, 0 }  }, /* 20 */
			{  is_last: 1 }		  /* last element    */
		}
	},
	{
		TST_ISW_REC(eucJP, alpha)
		{
			{  { 0x3000 }, { 0, 1, 0 }
			},  /* IDEO. SPACE	*/

			{  { 0x3020 }, { 0, 1, 0 }  }, /* POSTAL MARK FACE	*/
#ifdef SHOJI_IS_RIGHT
			{  { 0x3029 }, { 0, 1, 0 }  }, /* Hangzhou NUM9	*/
#else
			{  { 0x3029 }, { 0, 0, 0 }  }, /* Hangzhou NUM9	*/
#endif
			{  { 0x302F }, { 0, 1, 0 }  }, /* Diacritics(Hangul) */
			{  { 0x3037 }, { 0, 1, 0 }  }, /* Separator Symbol	*/
			{  { 0x303F }, { 0, 1, 0 }  }, /* IDEO. HALF SPACE	*/
#ifdef SHOJI_IS_RIGHT
			{  { 0x3041 }, { 0, 1, 0 }  }, /* HIRAGANA a		*/
			{  { 0x3094 }, { 0, 1, 0 }  }, /* HIRAGANA u"		*/
#else
			{  { 0x3041 }, { 0, 0, 0 }  }, /* HIRAGANA a		*/
			{  { 0x3094 }, { 0, 0, 0 }  }, /* HIRAGANA u"		*/
#endif
			{  { 0x3099 }, { 0, 1, 0 }  }, /* SOUND MARK		*/
#ifdef SHOJI_IS_RIGHT
			{  { 0x309E }, { 0, 1, 0 }  }, /* ITERATION MARK	*/
			{  { 0x30A1 }, { 0, 1, 0 }  }, /* KATAKANA a		*/
			{  { 0x30FA }, { 0, 1, 0 }  }, /* KATAKANA wo"		*/
#else
			{  { 0x309E }, { 0, 0, 0 }  }, /* ITERATION MARK	*/
			{  { 0x30A1 }, { 0, 0, 0 }  }, /* KATAKANA a		*/
			{  { 0x30FA }, { 0, 0, 0 }  }, /* KATAKANA wo"		*/
#endif
			{  { 0x30FB }, { 0, 1, 0 }  }, /* KATAKANA MID.DOT	*/
#ifdef SHOJI_IS_RIGHT
			{  { 0x30FE }, { 0, 1, 0 }  }, /* KATAKANA ITERATION	*/
#else
			{  { 0x30FE }, { 0, 0, 0 }  }, /* KATAKANA ITERATION	*/
#endif
			{  { 0x3191 }, { 0, 1, 0 }  }, /* KANBUN REV.MARK	*/
			{  { 0x3243 }, { 0, 1, 0 }  }, /* IDEO. MARK (reach) */
			{  { 0x32CB }, { 0, 1, 0 }  }, /* IDEO.TEL.SYM.DEC12 */
			{  { 0x32FE }, { 0, 1, 0 }  }, /* MARU KATAKANA wo	*/
			{  { 0x33FE }, { 0, 1, 0 }  }, /* CJK IDEO.TEL.31th	*/
#ifdef SHOJI_IS_RIGHT
			{  { 0x4E00 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x4E05 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x4E06 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.NON-J	*/
			{  { 0x4E07 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x4FFF }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x9000 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x9006 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x9007 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.NON-J	*/
			{  { 0x9FA4 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.NON-J	*/
			{  { 0x9FA5 }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
#else
			{  { 0x4E00 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x4E05 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x4E06 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.NON-J	*/
			{  { 0x4E07 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x4FFF }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x9000 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x9006 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0x9007 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.NON-J	*/
			{  { 0x9FA4 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.NON-J	*/
			{  { 0x9FA5 }, { 0, 0, 0 }  }, /* CJK UNI.IDEO.	*/
#endif
			{  { 0xFE4F }, { 0, 1, 0 }  }, /* CJK UNI.IDEO.	*/
			{  { 0xFF0F }, { 0, 1, 0 }  }, /* FULL SLASH		*/
#ifdef SHOJI_IS_RIGHT
			{  { 0xFF19 }, { 0, 1, 0 }  }, /* FULL 9		*/
#else
			{  { 0xFF19 }, { 0, 0, 0 }  }, /* FULL 9		*/
#endif
			{  { 0xFF20 }, { 0, 1, 0 }  }, /* FULL @		*/
			{  { 0xFF3A }, { 0, 0, 0 }  }, /* FULL Z		*/
			{  { 0xFF40 }, { 0, 1, 0 }  }, /* FULL GRAVE ACC.	*/
			{  { 0xFF5A }, { 0, 0, 0 }  }, /* FULL z		*/
			{  { 0xFF5E }, { 0, 1, 0 }  }, /* FULL ~ (tilde)	*/
			{  { 0xFF61 }, { 0, 1, 0 }  }, /* HALF IDEO.STOP. .	*/
			{  { 0xFF65 }, { 0, 1, 0 }  }, /* HALF KATA MID.DOT	*/
#ifdef SHOJI_IS_RIGHT
			{  { 0xFF66 }, { 0, 1, 0 }  }, /* HALF KATA WO		*/
			{  { 0xFF6F }, { 0, 1, 0 }  }, /* HALF KATA tu		*/
			{  { 0xFF70 }, { 0, 1, 0 }  }, /* HALF KATA PL -	*/
			{  { 0xFF71 }, { 0, 1, 0 }  }, /* HALF KATA A		*/
			{  { 0xFF9E }, { 0, 1, 0 }  }, /* HALF KATA MI		*/
#else
			{  { 0xFF66 }, { 0, 0, 0 }  }, /* HALF KATA WO		*/
			{  { 0xFF6F }, { 0, 0, 0 }  }, /* HALF KATA tu		*/
			{  { 0xFF70 }, { 0, 0, 0 }  }, /* HALF KATA PL -	*/
			{  { 0xFF71 }, { 0, 0, 0 }  }, /* HALF KATA A		*/
			{  { 0xFF9E }, { 0, 0, 0 }  }, /* HALF KATA MI		*/
#endif
			{  is_last: 1 }		  /* last element    */
		}
	},
	{	TST_ISW_REC(end, alpha) }
};
