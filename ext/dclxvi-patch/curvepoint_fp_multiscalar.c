/*
 * File:   dclxvi-20111122/curvepoint_fp_multiscalar.c
 * Author: Ruben Niederhagen, Peter Schwabe
 * Public Domain
 */

#include "curvepoint_fp.h"
#include "scalar.h"
#include "index_heap.h"

#define MAX_HEAP_SIZE 8191
void curvepoint_fp_multiscalarmult_vartime(curvepoint_fp_t rop, struct curvepoint_fp_struct *p, scalar_t *s, const unsigned long long npoints)
{
  unsigned long long pos[MAX_HEAP_SIZE];
  unsigned long long max1, max2,i;
  curvepoint_fp_t t;
  unsigned long long tctr, ctr = npoints;

  curvepoint_fp_setneutral(rop);
  while(ctr>=MAX_HEAP_SIZE)
  {
    heap_init(pos, MAX_HEAP_SIZE, s);
    for(i=0;;i++)
    {
      heap_get2max(pos, &max1, &max2, s);
      if(scalar_iszero_vartime(s[max2])) break;
      scalar_sub_nored(s[max1],s[max1],s[max2]);
      curvepoint_fp_add_vartime(&p[max2],&p[max2],&p[max1]);
      heap_rootreplaced(pos, MAX_HEAP_SIZE, s);
    }
    curvepoint_fp_scalarmult_vartime(t, &p[max1], s[max1]);
    curvepoint_fp_add_vartime(rop,rop,t);
    p += MAX_HEAP_SIZE;
    s += MAX_HEAP_SIZE;
    ctr -= MAX_HEAP_SIZE;
  }
  if(ctr > 5)
  {
    tctr = (ctr-1)|1; // need an odd heap size 
    heap_init(pos, tctr, s);
    for(i=0;;i++)
    {
      heap_get2max(pos, &max1, &max2, s);
      if(scalar_iszero_vartime(s[max2])) break;
      scalar_sub_nored(s[max1],s[max1],s[max2]);
      curvepoint_fp_add_vartime(&p[max2],&p[max2],&p[max1]);
      heap_rootreplaced(pos, tctr, s);
    }
    curvepoint_fp_scalarmult_vartime(t, &p[max1], s[max1]);
    curvepoint_fp_add_vartime(rop,rop,t);
    p += tctr;
    s += tctr;
    ctr -= tctr;
  }
  while(ctr>0)
  {
    curvepoint_fp_scalarmult_vartime(t,p,*s);
    curvepoint_fp_add_vartime(rop,rop,t);
    p++;
    s++;
    ctr--;
  }
}

