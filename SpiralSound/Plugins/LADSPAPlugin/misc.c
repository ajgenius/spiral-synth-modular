/* misc.c

   Free software by Richard W.E. Furse. Do with as you will. No
   warranty. */

/*****************************************************************************/

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/

#include "ladspa.h"
#include "utils.h"

/*****************************************************************************/

unsigned long
getPortCountByType(const LADSPA_Descriptor     * psDescriptor,
                   const LADSPA_PortDescriptor   iType) {
  unsigned long lCount;
  unsigned long lIndex;

  lCount = 0;
  for (lIndex = 0; lIndex < psDescriptor->PortCount; lIndex++)
    if ((psDescriptor->PortDescriptors[lIndex] & iType) == iType)
      lCount++;

  return lCount;
}
