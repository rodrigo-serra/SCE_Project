#include "mcc_generated_files/eusart.h"
#include "coms.h"
#include "globalvariables.h"
#include "mcc_generated_files/memory.h"

void readbytes(void){
    uint8_t data;
    
    data = EUSART_Read();
    if(data == SOM){ //Iniciar o armazenamento da msg ao reconhecer o inicio da mesma
        msg_array[pointer_array]=data;
        pointer_array++;
    }else if(pointer_array > 0){ //SOM ja foi recebido. Armazenar o resto da msg
        msg_array[pointer_array]=data;
        pointer_array++;
        if(data == EOM){ // Fim da msg, avaliar o conteudo
            interp_msg(); //Interpretar a msg
            pointer_array = 0;
        }
    } 
        
}

void interp_msg(void){
    
    int cmd;
    int data;
    int n;
    int iread;
    int numregs;
    
    cmd = msg_array[1]; //Leitura do byte corresponde ao comando pretendido
    writebytes(SOM);
    writebytes(msg_array[1]);
    
    
    switch(cmd) {
        case RCLK :
            //writebytes(16); //Enviar horas
            //writebytes(31); //Enviar minutos
            //writebytes(45); //Enviar segundos
            
            writebytes(hrs);
            writebytes(mins);
            writebytes(secs);
            break;
        case SCLK :
            //Atualiza Relogio
            hrs = msg_array[2];
            mins = msg_array[3];
            secs = msg_array[4];
            DATAEE_WriteByte(HOUR, msg_array[2]);
            DATAEE_WriteByte(MINUTE, msg_array[3]);
            //writebytes(17);
            break;
        case RTL :
            //Enviar temperatura e luminosidade
            writebytes(temperature);
            writebytes(luminosity);
            
            //writebytes(18);
            break;
        case RPAR :
            //Ler parametros PMON e TALA
            writebytes(PMON);
            writebytes(TALA);
            
            //writebytes(19);
            break;
        case MMP :
            //Atualiza PMON, periodo de registo
            PMON = msg_array[2];
            
            //writebytes(20);
            break;
        case MTA :
            //Atualiza TALA, duracao alarme
            TALA = msg_array[2];
            
            //writebytes(21);
            break;
        case RALA :
            //Leitura de thresholds e flag alarm
            writebytes(TempThreshold);
            writebytes(LumThreshold);
            writebytes(ALAF);
            
            //writebytes(22);
            break;
        case DATL :
            //Define thresholds
            TempThreshold = msg_array[2];
            LumThreshold = msg_array[3];
            DATAEE_WriteByte(THRESHLUM, msg_array[3]);
            DATAEE_WriteByte(THRESHTEMP, msg_array[2]);
            
            //writebytes(23);
            break;
        case AALA :
            //Altera alarm flag ALAF
            ALAF = msg_array[2];
            
            //writebytes(24);
            break;
        case RMM :
            //Ler registos relativos ao maximo e minimo
            //Envia 5 bytes relativos h,m,s,maxTemp,luminosidade
            data = DATAEE_ReadByte(MAXTEMP + HOURS);
            writebytes(data);
            data = DATAEE_ReadByte(MAXTEMP + MINUTES);
            writebytes(data);
            data = DATAEE_ReadByte(MAXTEMP + SECONDS);
            writebytes(data);
            data = DATAEE_ReadByte(MAXTEMP + TEMP);
            writebytes(data);
            data = DATAEE_ReadByte(MAXTEMP + LUM);
            writebytes(data);
            
            //Envia 5 bytes relativos h,m,s,Temp,Maxluminosidade
            data = DATAEE_ReadByte(MAXLUM + HOURS);
            writebytes(data);
            data = DATAEE_ReadByte(MAXLUM + MINUTES);
            writebytes(data);
            data = DATAEE_ReadByte(MAXLUM + SECONDS);
            writebytes(data);
            data = DATAEE_ReadByte(MAXLUM + TEMP);
            writebytes(data);
            data = DATAEE_ReadByte(MAXLUM + LUM);
            writebytes(data);
            
            //Envia 5 bytes relativos h,m,s,MINTemp,luminosidade
            data = DATAEE_ReadByte(MINTEMP + HOURS);
            writebytes(data);
            data = DATAEE_ReadByte(MINTEMP + MINUTES);
            writebytes(data);
            data = DATAEE_ReadByte(MINTEMP + SECONDS);
            writebytes(data);
            data = DATAEE_ReadByte(MINTEMP + TEMP);
            writebytes(data);
            data = DATAEE_ReadByte(MINTEMP + LUM);
            writebytes(data);
            
            //Envia 5 bytes relativos h,m,s,Temp,MINluminosidade
            data = DATAEE_ReadByte(MINLUM + HOURS);
            writebytes(data);
            data = DATAEE_ReadByte(MINLUM + MINUTES);
            writebytes(data);
            data = DATAEE_ReadByte(MINLUM + SECONDS);
            writebytes(data);
            data = DATAEE_ReadByte(MINLUM + TEMP);
            writebytes(data);
            data = DATAEE_ReadByte(MINLUM + LUM);
            writebytes(data);
            
            
            //writebytes(25);
            break;
        case CMM :
            //Clear dos registos maximo e minimo
            DATAEE_WriteByte(MAXTEMP + TEMP, 0);
            DATAEE_WriteByte(MAXLUM + LUM, 0);
            DATAEE_WriteByte(MINTEMP + TEMP, 200);
            DATAEE_WriteByte(MINLUM + LUM, 5);
            
            //writebytes(26);
            break;
        case IREG : 
            //Informacao sobre os registos
            //NREG
            writebytes(20);
            //Registos validos
            writebytes(DATAEE_ReadByte(NUM_REGS_SAVED));
            //iread
            writebytes(DATAEE_ReadByte(LAST_READ));
            //iwrite
            writebytes(DATAEE_ReadByte(LAST_WRITTEN));
            
            //writebytes(27);
            break;
        case TRGC :
            //Transferencia de n registos a partir da posicao atual de leitura
            n = msg_array[2];
            iread = DATAEE_ReadByte(LAST_READ);
            numregs = DATAEE_ReadByte(NUM_REGS_SAVED);
            
            if(n>numregs){
                n = numregs;
            }else if(n<0){
                n = 0;
            }
            writebytes(n);
            
            for(int i = 0; i < n ; i++)
            {
                iread += 1;
                if(iread >= 20)
                {
                    iread = 0;
                }
                
                data = DATAEE_ReadByte(START_INDEX + iread*5 + HOURS);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + MINUTES);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + SECONDS);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + TEMP);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + LUM);
                writebytes(data);
            }
            DATAEE_WriteByte(LAST_READ, iread);
            //writebytes(28);
            break;
        case TRGI :
            //Transferencia de n registos a partir da posicao i de leitura 
            n = msg_array[2];
            iread = msg_array[3];
            numregs = DATAEE_ReadByte(NUM_REGS_SAVED);
            
            if(n>numregs){
                n = numregs;
            }else if(n<0){
                n = 0;
            }
            writebytes(n);
            if(iread<0 || iread>20)
                iread=0;
                
            writebytes(iread);
            for(int i = 0; i < n ; i++)
            {
                iread += 1;
                if(iread >= 20)
                {
                    iread = 0;
                }
                
                data = DATAEE_ReadByte(START_INDEX + iread*5 + HOURS);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + MINUTES);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + SECONDS);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + TEMP);
                writebytes(data);
                data = DATAEE_ReadByte(START_INDEX + iread*5 + LUM);
                writebytes(data);
            }
            DATAEE_WriteByte(LAST_READ, iread);
            //writebytes(29);
            break;
        case NMFL :
            //Notificação de metade da memoria preenchida
            
            writebytes(30);
            break;
        default :
            writebytes(31);
    }
    writebytes(EOM);
}

void writebytes(uint8_t data){
    
    if(EUSART_is_tx_ready() >= sizeof(data)){
        EUSART_Write(data);
    }
    
}


