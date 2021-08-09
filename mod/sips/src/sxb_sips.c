#include "sxb_sips.h"

#include "fw/mxml/inc/sxb_api.ic"
#include "sxb_sips.ih"
#include "fw/mxml/inc/sxb_end.ih"


/*
 * 'whitespace_cb()' - Let the mxmlSaveFile() function know when to insert
 *                     newlines and tabs...
 */

const char *				/* O - Whitespace string or NULL */
whitespace_cb(mxml_node_t *node,	/* I - Element node */
              int         where)	/* I - Open or close tag? */
{
  mxml_node_t	*parent;		/* Parent node */
  int		level;			/* Indentation level */
  const char	*name;			/* Name of element */
  static const char *tabs = "\t\t\t\t\t\t\t\t";
					/* Tabs for indentation */


 /*
  * We can conditionally break to a new line before or after any element.
  * These are just common HTML elements...
  */

  name = node->value.element.name;

  if (!strcmp(name, "html") || !strcmp(name, "head") || !strcmp(name, "body") ||
      !strcmp(name, "pre") || !strcmp(name, "p") ||
      !strcmp(name, "h1") || !strcmp(name, "h2") || !strcmp(name, "h3") ||
      !strcmp(name, "h4") || !strcmp(name, "h5") || !strcmp(name, "h6"))
  {
   /*
    * Newlines before open and after close...
    */

    if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_AFTER_CLOSE)
      return ("\n");
  }
  else if (!strcmp(name, "dl") || !strcmp(name, "ol") || !strcmp(name, "ul"))
  {
   /*
    * Put a newline before and after list elements...
    */

    return ("\n");
  }
  else if (!strcmp(name, "dd") || !strcmp(name, "dt") || !strcmp(name, "li"))
  {
   /*
    * Put a tab before <li>'s, <dd>'s, and <dt>'s, and a newline after them...
    */

    if (where == MXML_WS_BEFORE_OPEN)
      return ("\t");
    else if (where == MXML_WS_AFTER_CLOSE)
      return ("\n");
  }
  else if (!strcmp(name, "?xml"))
  {
    return (NULL);
  }
  else if (where == MXML_WS_BEFORE_OPEN ||
           ((!strcmp(name, "choice") || !strcmp(name, "option")) &&
	    where == MXML_WS_BEFORE_CLOSE))
  {
    for (level = -1, parent = node->parent;
         parent;
	 level ++, parent = parent->parent);

    if (level > 8)
      level = 8;
    else if (level < 0)
      level = 0;

    return (tabs + 8 - level);
  }
  else if (where == MXML_WS_AFTER_CLOSE ||
           ((!strcmp(name, "group") || !strcmp(name, "option") ||
	     !strcmp(name, "choice")) &&
            where == MXML_WS_AFTER_OPEN))
    return ("\n");
  else if (where == MXML_WS_AFTER_OPEN && !node->child)
    return ("\n");

 /*
  * Return NULL for no added whitespace...
  */

  return (NULL);
}

