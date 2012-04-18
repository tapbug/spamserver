
m4_define([CONFIGURE_EMAIL], esyscmd([echo -n `cat ../debian/changelog | grep "^\ --\ " | head -n1 | cut -d\< -f2 | cut -d\> -f1`]))
m4_define([CONFIGURE_VERSION], esyscmd([echo -n `head -n1 ../debian/changelog | cut -d\( -f2 | cut -d\) -f1`]))

