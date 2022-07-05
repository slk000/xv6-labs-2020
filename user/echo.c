#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// replace escape char: "\n" -> \n
// return 1: '\' 'n' -> '\n'
int escape(char *str) {
  size_T cntReducedChar = 0;
  char *pOrigin = str;
  char *pDest = pOrigin;

  for(;*pOrigin != '\0'; pOrigin++) {
    // copy normal char
    if (*pOrigin == '"') {
      cntReducedChar++;
      continue;  //skip the enclosed " ", escaped " " is handled in switch
    }
    if (*pOrigin != '\\') {
      *pDest = *pOrigin;
      pDest++;
      continue;
    }
    // deal with escape
    pOrigin++;
    switch(*pOrigin) {
      case 'n':
        *pDest = '\n';
        break;
      case '\\':
        *pDest = '\\';
        break;
      case 't':
        *pDest = '\t';
        break;
      case '"':
        *pDest = '"';
        break;
      case '\0': // "\ followed by a \0" 
      default:
        write(1, "bad escape\n", 11);
        exit(1);
    }
    pDest++;
    cntReducedChar++;
  }
  pDest = '\0';
  return cntReducedChar;
}

int
main(int argc, char *argv[])
{
  int i;

  for(i = 1; i < argc; i++){
    char *str = argv[i];
    size_T argLen = strlen(str);
    size_T cntReducedChar = 0;
    if (argv[i][0] == '"' && argv[i][argLen-1] == '"') { // only escape for arg enclosed by " "
      cntReducedChar = escape(argv[i]);
    }
    
    write(1, str, argLen-cntReducedChar);
    if(i + 1 < argc){
      write(1, " ", 1);
    } else {
      write(1, "\n", 1);
    }
  }
  exit(0);
}
