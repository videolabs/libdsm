
#ifndef __SPNEGO_ASN1_CONF_H_
#define __SPNEGO_ASN1_CONF_H_

#include "config.h"

#include <libtasn1.h>

/* From tasn1 NEWS file */
/* 66   ASN1_DATA_NODE -> asn1_data_node_st */
/* 67   ASN1_ARRAY_TYPE -> asn1_static_node */
/* 68   ASN1_TYPE -> asn1_node */
/* 69   ASN1_TYPE_EMPTY -> NULL */
/* 70   static_struct_asn -> asn1_static_node_st */
/* 71   node_asn_struct -> asn1_node_st */
/* 72   node_asn -> asn1_node_st */
#ifndef HAVE_TASN1_3PLUS
# define asn1_static_node   ASN1_ARRAY_TYPE
# define asn1_node          ASN1_TYPE
#endif

extern const asn1_static_node spnego_asn1_conf[];

#endif
