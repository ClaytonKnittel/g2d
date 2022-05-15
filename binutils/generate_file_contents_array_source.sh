# Generates source code for a binary array with label ARR_LABEL and contents
# ARR_CONTENTS. These both must be passed to the compiler as definitions.
#
# ARR_CONTENTS must be of the form <hex_num>,<hex_num>,..., where each of
# <hex_num> are hexadecimal numbers from 0x00 - 0xff

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ]; then
	echo "usage: $0 <output_file> <input_file> <arr_label>"
	exit -1
fi

ENCODED_FILE_CONTENTS=$(od -t x1 -A n -v $2 | awk '{for(i=1;i<=NF;++i)printf("0x%s,", $i);}')

TEMPLATE_FILE="
#include <stddef.h>
#include <stdint.h>

static const uint8_t $3_tmp[] = {
${ENCODED_FILE_CONTENTS}
};

const void* const $3 = (const void*) $3_tmp;
const size_t $3_size = sizeof($3_tmp);
"

echo "$TEMPLATE_FILE" > $1

