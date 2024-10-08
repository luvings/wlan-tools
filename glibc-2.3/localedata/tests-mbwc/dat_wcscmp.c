/*
 *  TEST SUITE FOR MB/WC FUNCTIONS IN C LIBRARY
 *
 *	 FILE: dat_wcscmp.c
 *
 *	 WCSCMP:  int  wcscmp (const wchar_t *ws1, const wchar_t *ws2);
 */

/* NOTE:
	This is not a locale sensitive function and
	it may not make sence testing it for each locale ...
*/


TST_WCSCMP tst_wcscmp_loc [] = {
	{
		{ Twcscmp, TST_LOC_de },
		{
			{ /*input.*/ { { 0x00D1, 0x00D2, 0x00D3, 0x0000 },
					{ 0x00D1, 0x00D2, 0x00D3, 0x0000 },
				},  /* #1 */
				/*expect*/ { 0, 1, 0,			  },
			},
			{ /*input.*/ { { 0x0000, 0x00D1, 0x00D3, 0x0000 },
					{ 0x0000, 0x00D2, 0x00D3, 0x0000 },
				},  /* #2 */
				/*expect*/ { 0, 1, 0,			  },
			},
			{ /*input.*/ { { 0x00D1, 0x00D1, 0x00D3, 0x0000 },
					{ 0x0000, 0x00D2, 0x00D3, 0x0000 },
				},  /* #3 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ /*input.*/ { { 0x0000, 0x00D2, 0x00D3, 0x0000 },
					{ 0x00D1, 0x00D1, 0x00D3, 0x0000 },
				},  /* #4 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x00D1, 0x00D5, 0x00D3, 0x0000 },
					{ 0x00D1, 0x00D2, 0x00D3, 0x0000 },
				},  /* #5 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ /*input.*/ { { 0x00D1, 0x00D2, 0x00D3, 0x0000 },
					{ 0x00D1, 0x00D2, 0x00D9, 0x0000 },
				},  /* #6 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x00D1, 0x00D2, 0x0000	       },
					{ 0x00D1, 0x00D2, 0x00D9, 0x0000 },
				},  /* #7 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x00D1, 0x00D2, 0x00D9, 0x0000 },
					{ 0x00D1, 0x00D2, 0x0000	       },
				},  /* #8 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ is_last: 1 }
		}
	},
	{
		{ Twcscmp, TST_LOC_enUS },
		{
			{ /*input.*/ { { 0x0041, 0x0042, 0x0043, 0x0000 },
					{ 0x0041, 0x0042, 0x0043, 0x0000 },
				},  /* #1 */
				/*expect*/ { 0, 1, 0,			  },
			},
			{ /*input.*/ { { 0x0000, 0x0041, 0x0043, 0x0000 },
					{ 0x0000, 0x0042, 0x0043, 0x0000 },
				},  /* #2 */
				/*expect*/ { 0, 1, 0,			  },
			},
			{ /*input.*/ { { 0x0041, 0x0041, 0x0043, 0x0000 },
					{ 0x0000, 0x0042, 0x0043, 0x0000 },
				},  /* #3 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ /*input.*/ { { 0x0000, 0x0042, 0x0043, 0x0000 },
					{ 0x0041, 0x0041, 0x0043, 0x0000 },
				},  /* #4 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x0041, 0x0045, 0x0043, 0x0000 },
					{ 0x0041, 0x0042, 0x0043, 0x0000 },
				},  /* #5 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ /*input.*/ { { 0x0041, 0x0042, 0x0043, 0x0000 },
					{ 0x0041, 0x0042, 0x0049, 0x0000 },
				},  /* #6 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x0041, 0x0042, 0x0000	       },
					{ 0x0041, 0x0042, 0x0049, 0x0000 },
				},  /* #7 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x0041, 0x0042, 0x0049, 0x0000 },
					{ 0x0041, 0x0042, 0x0000	       },
				},  /* #8 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ is_last: 1 }
		}
	},
	{
		{ Twcscmp, TST_LOC_eucJP},
		{
			{ /*input.*/ { { 0x3041, 0x3042, 0x3043, 0x0000 },
					{ 0x3041, 0x3042, 0x3043, 0x0000 },
				},  /* #1 */
				/*expect*/ { 0, 1, 0,			  },
			},
			{ /*input.*/ { { 0x0000, 0x3041, 0x3043, 0x0000 },
					{ 0x0000, 0x3042, 0x3043, 0x0000 },
				},  /* #2 */
				/*expect*/ { 0, 1, 0,			  },
			},
			{ /*input.*/ { { 0x3041, 0x3041, 0x3043, 0x0000 },
					{ 0x0000, 0x3042, 0x3043, 0x0000 },
				},  /* #3 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ /*input.*/ { { 0x0000, 0x3042, 0x3043, 0x0000 },
					{ 0x3041, 0x3041, 0x3043, 0x0000 },
				},  /* #4 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x3041, 0x3045, 0x3043, 0x0000 },
					{ 0x3041, 0x3042, 0x3043, 0x0000 },
				},  /* #5 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ /*input.*/ { { 0x3041, 0x3042, 0x3043, 0x0000 },
					{ 0x3041, 0x3042, 0x3049, 0x0000 },
				},  /* #6 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x3041, 0x3042, 0x0000	       },
					{ 0x3041, 0x3042, 0x3049, 0x0000 },
				},  /* #7 */
				/*expect*/ { 0, 1, -1,			  },
			},
			{ /*input.*/ { { 0x3041, 0x3042, 0x3049, 0x0000 },
					{ 0x3041, 0x3042, 0x0000	       },
				},  /* #8 */
				/*expect*/ { 0, 1, 1,			  },
			},
			{ is_last: 1 }
		}
	},
	{
		{ Twcschr, TST_LOC_end}
	}
};
