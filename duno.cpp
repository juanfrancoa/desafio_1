#include "duno.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

unsigned char** tableroBytes(unsigned int i, unsigned int j, unsigned int verficar){
    unsigned char fichas[8]={0b000,0b001,0b010,0b011,0b100,0b101,0b110,0b111};
    unsigned char** bytesTablero= new unsigned char*[i];
    for(unsigned int f=0;f<i;f++){
        bytesTablero[f]=new unsigned char[j];
    }
    unsigned int bitsTotales=0;
    unsigned int contador3Bits=1;
    unsigned char byte=0;
    unsigned char primerBit=0;
    unsigned char primerosdosBits=0;
    unsigned char ultimosdosBits=0;
    unsigned char ultimoBit=0;
    unsigned int contadorBits=0;
    unsigned int posicion=0;
    unsigned char ficha=0;
    unsigned int F=0;
    unsigned int c=0;
    while(bitsTotales<(i*j)){
        if(contador3Bits==1){
            unsigned int contadorBit=0;
            while(contadorBit!=2){
                if(bitsTotales>=(i*j)){bytesTablero[F][c]=byte;goto fin;}
                ficha=fichas[0+rand()%(7-0+1)];
                byte|=(ficha<<posicion);
                posicion+=3;
                contadorBit++;
                bitsTotales++;

            }
            if(bitsTotales>=(i*j)){bytesTablero[F][c]=byte;goto fin;}
            ficha=fichas[0+rand()%(7-0+1)];
            primerosdosBits= (ficha >> 1) & 0b11;
            byte|=(primerosdosBits<<posicion);
            ultimoBit=ficha & 0b1;
            contadorBits=0;
            bytesTablero[F][c]=byte;
            byte=0;
            posicion=0;
            c++;
            if(c==j){c=0;F++;}
        }
        if(contador3Bits==2){
            if(contadorBits==0){
                byte|=(ultimoBit<<posicion);
                posicion++;
                bitsTotales++;
                contadorBits++;
            }
            if(contadorBits==1){
                if(bitsTotales>=(i*j)){bytesTablero[F][c]=byte;goto fin;}
                unsigned int contadorBit=0;
                while(contadorBit!=2){
                    if(bitsTotales>=(i*j)){bytesTablero[F][c]=byte;goto fin;}
                    ficha=fichas[0+rand()%(7-0+1)];
                    byte|=(ficha<<posicion);
                    posicion+=3;
                    bitsTotales++;
                    contadorBit++;
                }
                if(bitsTotales>=(i*j)){bytesTablero[F][c]=byte;goto fin;}
                ficha=fichas[0+rand()%(7-0+1)];
                primerBit=(ficha >> 2) & 0b1;
                byte|=(primerBit<<posicion);
                bytesTablero[F][c]=byte;
                byte=0;
                posicion=0;
                ultimosdosBits= ficha & 0b11;
                c++;
                if(c==j){c=0;F++;}
            }
        }
        if(contador3Bits==3){
            byte|=(ultimosdosBits<<posicion);
            bitsTotales++;
            posicion+=2;
            unsigned int contadorBit=0;
            while(contadorBit!=2){
                if(bitsTotales>=(i*j)){bytesTablero[F][c]=byte;goto fin;}
                ficha=fichas[0+rand()%(7-0+1)];
                byte|=(ficha<<posicion);
                posicion+=3;
                bitsTotales++;
                contadorBit++;
            }
            bytesTablero[F][c]=byte;
            posicion=0;
            contador3Bits=0;
            c++;
            if(c==j){c=0;F++;}
        }
        contador3Bits++;
    }
fin:
    if(bitsTotales==verficar){
        cout<<"se reservaron las fichas en bytes correctamente";
    }
    return bytesTablero;
}
void bitAficha(unsigned char bits){
    switch(bits){
    case 0b000:
        cout<<"# ";
        break;
    case 0b001:
        cout<<"$ ";
        break;
    case 0b010:
        cout<<"% ";
        break;
    case 0b011:
        cout<<"& ";
        break;
    case 0b100:
        cout<<"* ";
        break;
    case 0b101:
        cout<<"+ ";
        break;
    case 0b110:
        cout<<"y ";
        break;
    case 0b111:
        cout<<"x ";
        break;
    default:
        cout<<"programador el bit se ha formado mal =(";
    }
}
void imprimirTablero(unsigned char** ByteTablero, unsigned int i,unsigned int j, int verificar ){
    int contadorFichas=0;
    int contfichas=0;
    unsigned int contcolumnas=0;
    unsigned char ficha=0;
    unsigned short int calculofichas=1;
    unsigned int posicion=0;
    for(unsigned int f=0;f<i;f++){
        if(contadorFichas>=verificar){
            break;
        }
        for(unsigned int c=0;c<j;c++){
            if(contadorFichas>=verificar){
                break;
            }
            if(calculofichas==1){
                posicion=0;
                while(contfichas!=2){
                    if(contadorFichas>=verificar){
                        break;
                    }
                    ficha=(ByteTablero[f][c]>>posicion)&0b111;
                    contcolumnas++;
                    if(contcolumnas>=j){
                        contcolumnas=0;
                        cout<<endl;
                    }
                    bitAficha(ficha);
                    posicion+=3;
                    contadorFichas++;
                    contfichas++;
                }
                contfichas=0;
                if(contadorFichas>=verificar){
                    calculofichas++;
                    calculofichas++;
                    continue;
                }
                unsigned char primerosDosBits=(ByteTablero[f][c]>>6)&0b11;
                c++;
                unsigned char ultimoBit=ByteTablero[f][c]&0b1;
                ficha=0;
                ficha|=(primerosDosBits<<1);
                ficha|=ultimoBit;
                contcolumnas++;
                if(contcolumnas>=j){
                    contcolumnas=0;
                    cout<<endl;
                }
                bitAficha(ficha);
                contadorFichas++;
                calculofichas++;
            }
            if(calculofichas==2){
                posicion=1;
                while(contfichas!=2){
                    if(contadorFichas>=verificar){
                        break;
                    }
                    ficha=(ByteTablero[f][c]>>posicion)&0b111;
                    contcolumnas++;
                    if(contcolumnas>=j){
                        contcolumnas=0;
                        cout<<endl;
                    }
                    bitAficha(ficha);
                    posicion+=3;
                    contadorFichas++;
                    contfichas++;
                }
                contfichas=0;
                if(contadorFichas>=verificar){
                    calculofichas++;
                    continue;
                }
                unsigned char primerBit=(ByteTablero[f][c]>>7)&0b1;
                ficha=0;
                ficha|=(primerBit<<2);
                c++;
                unsigned char ultimosDosBits=ByteTablero[f][c]&0b11;
                ficha|=ultimosDosBits;
                contcolumnas++;
                if(contcolumnas>=j){
                    contcolumnas=0;
                    cout<<endl;
                }
                bitAficha(ficha);
                contadorFichas++;
                calculofichas++;
            }
            if(calculofichas==3){
                posicion=2;
                while(contfichas!=2){
                    if(contadorFichas>=verificar){
                        break;
                    }
                    ficha=(ByteTablero[f][c]>>posicion)&0b111;
                    contcolumnas++;
                    if(contcolumnas>=j){
                        contcolumnas=0;
                        cout<<endl;
                    }
                    bitAficha(ficha);
                    posicion+=3;
                    contadorFichas++;
                    contfichas++;
                }
                contfichas=0;
                calculofichas=0;
            }
            calculofichas++;
        }
    }
}