/*================================================================================================*/
/**
 * @file		version.h
 *
 * @brief		Version information - interface.
 *
 *------------------------------------------------------------------------------------------------*
 *
 * @author		Pavel Surynek
 *
 * @email		pavel.surynek@seznam.cz
 */
/*================================================================================================*/
/*                                                                                                *
 *    ********  REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW  ********    *
 *                                                                                                *
 *                                                                                                *
 *                             Copyright (C) 2003-2008 Pavel Surynek                              *
 *                                      All Rights Reserved                                       *
 *                                                                                                *
 *                                                                                                *
 *  The contents of this file  are subject to the author's Licence;  you may *not* use this file  *
 *  or related  software except  in compliance  with the Licence.  For more details see the file  *
 *                         called LICENCE in the root of the distribution.                        *
 *                                                                                                *
 *                                                                                                *
 *  Software distributed under the Licence  is distributed on an "AS IS" basis, WITHOUT WARRANTY  *
 *  OF ANY KIND, either  express or implied. See the Licence for the specific language governing  *
 *                            rights and limitations under the Licence.                           *
 *                                                                                                *
 *                                                                                                *
 *    ********  REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW  ********    *
 *                                                                                                */
/*================================================================================================*/
/*
 * $Id: version.h.in,v 1.3 2007/12/31 04:42:18 pavel Exp $
 */
/*================================================================================================*/




/*------------------------------------------------------------------------------------------------*/

#ifndef	__S_COMMON_VERSION_H__
#define	__S_COMMON_VERSION_H__






/*================================================================================================*/
// Definitions of product information macros
/*------------------------------------------------------------------------------------------------*/


#define PRODUCT			"@S_product@"
#define PACKAGE			"@PACKAGE@"
#define VERSION			"@S_version@"

#define AUTHOR			"@S_author@"
#define AUTHOR_LONG		"@S_author_long@"

#define sPRODUCT		PRODUCT
#define sPACKAGE		PACKAGE
#define sVERSION		VERSION

#define sAUTHOR			AUTHOR
#define sAUTHOR_LONG		AUTHOR_LONG




/*------------------------------------------------------------------------------------------------*/

#endif	/* !defined __S_COMMON_VERSION_H__ */




/*
 * EOF		version.h
 */
