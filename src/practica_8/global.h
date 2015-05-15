#ifndef __GLOBAL_STRUCTS_FLAGS__

  #define __GLOBAL_STRUCTS_FLAGS__ 1
  // ====== DEFINITIONS ======

  #define HEADS 24
  #define SECTORS 17
  #define CYLINDERS 80
  #define SECSIZE 512
  #define BLOCKSECT 8

  #define BLOCKSIZE (SECSIZE * BLOCKSECT)
  #define INODESIZE sizeof(struct INODE)
  #define NINODES 32

  #define DIRECTPTRxINODE 10
  #define PTRxBLOCK (BLOCKSIZE / sizeof(unsigned short))
  #define NOPENFILES 10

  #define ERROR -1
  #define SUCCESS 1

  #define YES 1
  #define NO 0

  #define WHENCE_BEG 0
  #define WHENCE_CUR 1
  #define WHENCE_END 2

  #define MAXLEN 80
  #define BUFFERSIZE 512

  #define DIRDEPTH 2
  // ====== STRUCTURES ======

  struct INODE {
    char name[20];
    unsigned short uid;
    unsigned short gid;
    unsigned short perms;
    unsigned int datetimecreate;
    unsigned int datetimemodif;
    unsigned int size;
    unsigned short blocks[10];
    unsigned short indirect;
    unsigned short indirect2;
  };


  struct SECBOOT {
    char jump[4];
    char nombre_disco[8];
    unsigned char sec_res;
    unsigned char sec_mapa_bits_nodo_i;
    unsigned char sec_mapa_bits_bloques;
    unsigned short sec_tabla_nodos_i;
    unsigned short sec_log_unidad;
    unsigned char sec_x_bloque;
    unsigned char heads;
    unsigned char cyls;
    unsigned char secfis;
    char restante[487];
  };


  struct DATE {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
  };


  struct OPENFILES {
    int inUse;
    unsigned short iNode;
    int currPos;
    int currBlockInMemory;
    char buffer[2048];
    unsigned short indirectBuff[1024];
  };


  typedef int VDDIR;


  struct vddirent {
    char *d_name;
  };

  // ====== SOME FLAGS AND DATA STRUCTURES =======

  extern short secBootInMemory;
  extern struct SECBOOT secBoot;

  extern short iNodesMapInMemory;
  extern unsigned char iNodesMap[];

  extern short dataMapInMemory;
  extern unsigned char dataMap[];

  extern short rootDirInMemory;
  extern struct INODE rootDir[];

  extern short openFilesInMemory;
  extern struct OPENFILES openfiles[];

  extern VDDIR dirs[DIRDEPTH];
  extern struct vddirent dirActual;
#endif
