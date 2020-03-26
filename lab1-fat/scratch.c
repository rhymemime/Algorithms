/*unsigned short * init_fat(char *diskfile){
 *     FILE * pFile;
 *         unsigned short* fat_table;
 *             size_t result;
 *                 pFile = fopen(diskfile, "rw");
 *                     if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
 *
 *                         int fatSize = fat_size(diskfile) * SIZE_S / sizeof(unsigned short);//need to double check on size...
 *
 *                             printf("readSize: %d\n\n", fatSize);
 *
 *                                 fat_table = (unsigned short*) malloc (sizeof(unsigned short)*fatSize);
 *
 *                                     memset(fat_table, 0, sizeof(unsigned short)*fatSize);
 *
 *                                         //result = fread (buffer,2,readSize,pFile); n
 *                                             //printf("buffer = %d\n", *buffer);
 *                                                 //printf("buffer = %d\n", *(buffer+1));
 *                                                     //printf("buffer = %d\n", *(buffer+2));
 *
 *                                                         //listEl = fread (buffer,2,1,pFile);
 *                                                             //
 *                                                                 return fat_table;
 *                                                                 }*/
