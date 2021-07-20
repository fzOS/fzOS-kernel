 #include <common/kstring.h>
int strcopy(char* dest,char* src,int n) {
    int count=0;
    for(;count<n;count++) {
        dest[count]=src[count];
    }
    dest[n]=0;
    return ++count;
}
int strcomp(char* first,char* second) {
    int result =0;
    int pointer=0;
    while((!result)&&first[pointer]&&second[pointer]) {
        result = second[pointer]-first[pointer];
        pointer++;
    }
    if(!result)
        result=first[pointer]?-1:(second[pointer]?1:0);
    return result;
}
