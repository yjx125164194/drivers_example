#!/bin/sh

prog_name="testdata"
tmp_name=/tmp/1.c
out_name="test_file"

rm -f ${prog_name}
sed "1,/^### END OF THE SCRIPT###/d" "$0" > ${prog_name} 
chmod u+x ${prog_name}
openssl enc -d -aes256 -in ${prog_name} -out ${tmp_name}
gcc ${tmp_name} -o ${out_name}
rm -f ${prog_name}
rm -f ${tmp_name}

### WARNING: Do not modify the following!!!
exit 0
### END OF THE SCRIPT###
