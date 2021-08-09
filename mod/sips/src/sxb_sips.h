#ifndef __sxb_sips_h__
#define __sxb_sips_h__

#undef __ELE_DEF
#undef ELE_NUM_SET
#undef ELE_NUM_GET
#define __ELE__

#include "fw/mxml/inc/sxb_api.ih"
#include "mod/sips/src/sxb_sips.ih"
#include "fw/mxml/inc/sxb_end.ih"


const char *				/* O - Whitespace string or NULL */
whitespace_cb(mxml_node_t *node,	/* I - Element node */
              int         where);	/* I - Open or close tag? */

#endif //__sxb_sips_h__