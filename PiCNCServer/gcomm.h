typedef struct {
    double PosX;
    double PosY;
    double PosZ;
    int spuX;
    int spuY;
    int spuZ;
    int F;
    int laserWidth;
    int burnTime;
} MSets_t;

int sets_fd;
char setsBuf[sizeof(MSets_t)];
MSets_t *datObj = (MSets_t*)setsBuf;

typedef struct GCommand {
    signed char GM[1];
    unsigned char num;
    unsigned char arr[];
} GCommand_t;

typedef struct G01 {
    long int x;
    long int y;
    long int z;
    unsigned long F;
    unsigned long acseleration;
} G01_t;

typedef struct C03 {
    size_t N;
    long int x;
    long int y;
    long int z;
    unsigned long int dx;
    unsigned long int dy;
    unsigned long int dz;
} C03_t;

typedef struct PXYZ{
    int x;
    int y;
    int z;
}PXYZ_t;
struct XYZActPos{
    double x;
    double y;
    double z;
}ActPos;
char posBuf[sizeof(PXYZ_t)];
PXYZ_t *dat = (PXYZ_t*)posBuf;
double XYZMsg[3];

int fd;

void ManualControl(unsigned char clMsg)
{
    uint16_t maxSPU = std::max(std::max(datObj->spuX, datObj->spuY), datObj->spuZ);
    char dataBuf[sizeof(GCommand_t)+sizeof(G01_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    G01_t *G01msg = (G01_t*)(msg->arr);
    msg->GM[0] = 'C';

    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        puts("Can't open file /dev/piCNC ");
    }

    if(clMsg & 0b1000000){
        msg->num = 12;
        write (fd, dataBuf, sizeof(GCommand_t));
    }else{
        msg->num = 1;

        G01msg->x = (clMsg & 0b000010)? -1 : (clMsg & 0b000001)? 1 : 0;
        G01msg->y = (clMsg & 0b001000)? -1 : (clMsg & 0b000100)? 1 : 0;
        G01msg->z = (clMsg & 0b100000)? -1 : (clMsg & 0b010000)? 1 : 0;

        G01msg->F = 30000000000 / (datObj->F * maxSPU);
        G01msg->acseleration = datObj->F * maxSPU / 100;

        write (fd, dataBuf, sizeof(GCommand_t) + sizeof(G01_t));
    }

    close(fd);
}

void GoToPos(){
    uint16_t maxSPU = std::max(std::max(datObj->spuX, datObj->spuY), datObj->spuZ);
    char dataBuf[sizeof(GCommand_t)+sizeof(C03_t)];
    GCommand_t *msg = (GCommand_t*)dataBuf;
    msg->GM[0] = 'C';

    if((fd = open("/dev/piCNC", O_RDWR)) < 0){
        puts("Can't open file /dev/piCNC ");
    }

    msg->num = 2;
    unsigned long *F = (unsigned long*)(msg->arr);
    *F = 30000000000 / (datObj->F * maxSPU);
    write (fd, dataBuf, sizeof(GCommand_t)+4);

    C03_t *C03msg = (C03_t*)(msg->arr);
    msg->num = 3;
    C03msg->x = (XYZMsg[0] - ActPos.x) * datObj->spuX;
    C03msg->y = (XYZMsg[1] - ActPos.y) * datObj->spuY;
    C03msg->z = (XYZMsg[2] - ActPos.z) * datObj->spuZ;

    double MaxDist = std::max(std::max(std::abs(C03msg->x), std::abs(C03msg->y)), std::abs(C03msg->z));
    if(MaxDist != 0){
        double DX = (double(C03msg->x) / double(MaxDist)) * 1048576;
        double DY = (double(C03msg->y) / double(MaxDist)) * 1048576;
        double DZ = (double(C03msg->z) / double(MaxDist)) * 1048576;//(1<<20) 1048576
        if(DX < 0) DX *= -1;
        if(DY < 0) DY *= -1;
        if(DZ < 0) DZ *= -1;
        C03msg->dx = DX;
        C03msg->dy = DY;
        C03msg->dz = DZ;
        write (fd, dataBuf, sizeof(GCommand_t)+sizeof(C03_t));
    }

    ActPos.x = XYZMsg[0];
    ActPos.y = XYZMsg[1];
    ActPos.z = XYZMsg[2];

    close(fd);
}
