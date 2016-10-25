/********
vcftool.h -- header file for vcftool.c in Asmt 2
Last updated:  Feb 10th, 2014 05:01:00 PM 

Student Name: Ben Ventresca			Student Number: 0823887
Date: Feb 10th, 2014					Course Name: CIS-2750
Contact: bventres@uoguelph.ca		Description: header file for vcf tools
*****************************************************************************/

#ifndef VCFTOOL_H_
#define VCFTOOL_H_ A2

#include "vcutil.h"
//#include <pcre.h>
#include <regex.h>
#include <sys/types.h>
#include "iso3166-2_CountryCodes.h"

int vcfInfo( FILE *const outfile, const VcFile *filep );
int vcfCanon( VcFile *const filep );
int vcfSelect( VcFile *const filep, const char *which );
int vcfSort( VcFile *const filep );

int vcfCanProp( VcProp *const propp );

#endif
