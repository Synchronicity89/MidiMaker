/**
 * \file wavHandler.c
 * \author brice
 * \date 19/02/19.
 * \headerfile wavHandler.h
 * \brief .wav files handler. That file aims at loading wav data in a C data structure.
 */

#include "wavHandler.h"

/**
 * \fn static int loadFmtChunk(FILE* file_pointer, WavHeader* header)
 * \author Brice Croix
 * \brief This function loads the content of a fmt chunk in the WavHeader structure, it is called by "load"
 * \param[in] file_pointer file pointer where to read the fmt chunk
 * \param[out] header address of the WavHeader struct where to store header data.
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
static int loadFmtChunk(FILE* file_pointer, WavHeader* header){
    __uint32_t fmtChunkSize;
    fread(&fmtChunkSize, sizeof(__uint32_t),1,file_pointer);
    if(fmtChunkSize<16){//That is not supposed to happen
        fprintf(stderr,"The fmt chunk length is inferior to 16 bytes. Could not load it.\n");
        fseek(file_pointer,fmtChunkSize,SEEK_CUR);
        return EXIT_FAILURE;
    }

    fread(&(header->wFormatTag), sizeof(__uint16_t),1,file_pointer);
    fread(&(header->nChannels), sizeof(__uint16_t),1,file_pointer);
    fread(&(header->nSamplesPerSec), sizeof(__uint32_t),1,file_pointer);
    fread(&(header->nAvgBytesPerSec), sizeof(__uint32_t),1,file_pointer);
    fread(&(header->nBlockAlign), sizeof(__uint16_t),1,file_pointer);
    fread(&(header->wBitsPerSample), sizeof(__uint16_t),1,file_pointer);

    if(fmtChunkSize==18){//For non-PCM data
        fread(&(header->cbSize), sizeof(__uint16_t),1,file_pointer);
    }else if(fmtChunkSize==40){//For Extensible-format data
        fread(&(header->cbSize), sizeof(__uint16_t),1,file_pointer);
        fread(&(header->wValidBitsPerSample), sizeof(__uint16_t),1,file_pointer);
        fread(&(header->dwChannelMask), sizeof(__uint32_t),1,file_pointer);
        fread(&(header->subFormat), sizeof(__uint16_t),1,file_pointer);
        fseek(file_pointer,14,SEEK_CUR); /*There are 14 bytes here but I don't know their purpose*/
    }else if(fmtChunkSize==16){//PCM-format fmt chunk is supposed to be 16 bytes long
        return EXIT_SUCCESS;
    }else{//If the length of this chunk is weird, let us go to its end
        fseek(file_pointer,fmtChunkSize-16,SEEK_CUR);
    }
    return EXIT_SUCCESS;

}

/**
 * \fn static int loadFactChunk(FILE* file_pointer, WavHeader* header)
 * \author Brice Croix
 * \brief This function loads the content of a fact chunk in the WavHeader structure, it is called by "load"
 * \param[in] file_pointer file pointer where to read the fact chunk
 * \param[out] header address of the WavHeader struct where to store header data.
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
static int loadFactChunk(FILE* file_pointer, WavHeader* header){
    __uint32_t factChunkSize;
    fread(&factChunkSize, sizeof(__uint32_t),1,file_pointer);
    if(factChunkSize<4){ //There is at least an integer stored in that chunk
        fprintf(stderr,"The fact chunk length is inferior to 4 bytes. Could not load it.\n");
        fseek(file_pointer,factChunkSize,SEEK_CUR);
        return EXIT_FAILURE;
    }

    fread(&(header->dwSampleLength), sizeof(__uint32_t),1,file_pointer);

    if(factChunkSize>4) fseek(file_pointer,factChunkSize-4,SEEK_CUR); //If the length of this chunk is weird, let us go to its end
    return EXIT_SUCCESS;
}

/**
 * \fn static __uint32_t loadDataChunk(FILE* file_pointer, WavHeader* header, int** samples)
 * \author Brice Croix
 * \brief This function loads the content of a data chunk in the samples array, it is called by "load"
 * \param[in] file_pointer file pointer where to read the data chunk
 * \param[out] header address of the WavHeader struct where to store header data.
 * \param[out] samples address of the int array where  to store the samples, dynamically allocated
 * \return number of samples successfully loaded.
 */
static __uint32_t loadDataChunk(FILE* file_pointer, WavHeader* header, int** samples){
    __uint32_t DataChunkSize; //size of the data chunk
    __uint32_t i; //iterator
    __uint32_t nb_samples_read; //number of samples successfully read
    __uint32_t samplesPerChannel; //useful to store since we need it multiple times.
    void* mono; //Array to store the samples temporarily when using a mono file
    void* channel1; //To store the samples temporarily when using a stereo file (not arrays)
    void* channel2; //To store the samples temporarily when using a stereo file (not arrays)
    __uint16_t avg8; //To temporarily store the sum of the channels (on 1 bytes)
    int32_t avg16; //To temporarily store the sum of the channels (on 2 bytes)
    int64_t avg32; //To temporarily store the sum of the channels (on 4 bytes)

    __uint8_t confirm_read;


    fread(&DataChunkSize, sizeof(__uint32_t),1,file_pointer);

    //update the possibly non initialized header fields.
    if(header->wValidBitsPerSample ==0) header->wValidBitsPerSample = header->wBitsPerSample;
    if(header->dwSampleLength ==0) header->dwSampleLength = DataChunkSize/((header->nChannels)*(header->wBitsPerSample /8));//How many samples per channel

    if(DataChunkSize==0){
        fprintf(stderr,"This file does not contain any data in its data Chunk.\n");
        return 0;
    }

    samplesPerChannel = header->dwSampleLength;

    *samples = (int*)safe_malloc(samplesPerChannel* sizeof(int));


    if(header->nChannels==1) {//If there is only one channel
        switch(header->wBitsPerSample){ //How many bits per sample?
            case 8: ;
                //In this case samples are represented as binary with an offset.
                mono = safe_malloc(samplesPerChannel*sizeof(__uint8_t));

                nb_samples_read = (__uint32_t)fread(mono, sizeof(__uint8_t),(size_t)samplesPerChannel,file_pointer);

                //Let's convert these sample to signed integers
                for(i=0;i<samplesPerChannel;i++){
                    (*samples)[i] = *((__uint8_t*)mono +i) - 0x80;
                }

                free(mono);
                break;

            case 16: ;
                //In this case samples are two's complement
                mono = (int16_t*)safe_malloc(samplesPerChannel*sizeof(int16_t));

                nb_samples_read = (__uint32_t)fread(mono, sizeof(__int16_t),(size_t)samplesPerChannel,file_pointer);

                //Let's convert these sample to signed integers.
                for(i=0;i<samplesPerChannel;i++){
                    (*samples)[i] = *((__int16_t*)mono +i);
                }

                free(mono);
                break;

            case 32:
                //In this case samples are two's complement, plus it is possible to load them directly.

                nb_samples_read = (__uint32_t)fread(*samples, sizeof(__int32_t),(size_t)samplesPerChannel,file_pointer);

                break;

            default:
                fprintf(stderr,"The samples of this file are stored on %d bits. Must be 8, 16 or 32.\n",header->wBitsPerSample);
                free(samples);
                nb_samples_read=0;
                break;
        }

    }else if(header->nChannels==2) {/*In this case we shall return for each sample the average of the channels*/
        nb_samples_read = 0;

        switch(header->wBitsPerSample) { //How many bits per sample?
            case 8: ;
                //In this case samples are represented as binary with an offset.
                channel1 = safe_malloc(sizeof(__uint8_t));
                channel2 = safe_malloc(sizeof(__uint8_t));
                for(i=0;i<samplesPerChannel;i++){
                    confirm_read = (__uint8_t)fread(channel1, sizeof(__uint8_t),1,file_pointer);
                    confirm_read+= (__uint8_t)fread(channel2, sizeof(__uint8_t),1,file_pointer);

                    nb_samples_read+=(confirm_read>>1);
                    avg8 = (*(__uint8_t*)channel1) + (*(__uint8_t*)channel2);
                    (*samples)[i] = avg8/2 - 0x80;
                }
                free(channel1);
                free(channel2);
                break;

            case 16: ;
                //In this case samples are two's complement
                channel1 = safe_malloc(sizeof(__int16_t));
                channel2 = safe_malloc(sizeof(__int16_t));
                for(i=0;i<samplesPerChannel;i++){
                    confirm_read = (__uint8_t)fread(channel1, sizeof(__int16_t),1,file_pointer);
                    confirm_read+= (__uint8_t)fread(channel2, sizeof(__int16_t),1,file_pointer);

                    nb_samples_read+=(confirm_read>>1);
                    avg16 = (*(__int16_t *)channel1) + (*(__int16_t *)channel2);
                    (*samples)[i] = avg16/2;
                }
                free(channel1);
                free(channel2);
                break;

            case 32: ;
                //In this case samples are two's complement
                channel1 = safe_malloc(sizeof(__int32_t));
                channel2 = safe_malloc(sizeof(__int32_t));
                for(i=0;i<samplesPerChannel;i++){
                    confirm_read = (__uint8_t)fread(channel1, sizeof(__int32_t),1,file_pointer);
                    confirm_read+= (__uint8_t)fread(channel2, sizeof(__int32_t),1,file_pointer);

                    nb_samples_read+=(confirm_read>>1);
                    avg32 = (*(__int32_t *)channel1) + (*(__int32_t *)channel2);
                    (*samples)[i] = (int)avg32/2;
                }
                free(channel1);
                free(channel2);
                break;

            default:
                fprintf(stderr,"The samples of this file are stored on %d bits. Must be 8, 16 or 32.\n",header->wBitsPerSample);
                free(samples);
                nb_samples_read = 0;
                break;
        }

    }else{
        fprintf(stderr,"This program can only process wav file with one or two channels.\n");
        free(samples);
        nb_samples_read=0;
    }

    if(DataChunkSize%2 !=0) fseek(file_pointer,1,SEEK_CUR);//There is a padding byte if this chunk size is odd
    return nb_samples_read;
}

/**
 * \fn initWavHeader(WavHeader* h)
 * \author Brice Croix
 * \brief Initialize all header fields at 0.
 * \param[out] h wavHeader pointer to be initialized
 */
void initWavHeader(WavHeader* h){
    h->file_size = 0;
    h->wFormatTag = WAVE_FORMAT_UNDEFINED;
    h->nChannels = 0;
    h->nSamplesPerSec = 0;
    h->nAvgBytesPerSec = 0;
    h->nBlockAlign = 0;
    h->wBitsPerSample = 0;
    h->cbSize = 0;
    h->wValidBitsPerSample = 0;
    h->dwChannelMask = 0;
    h->subFormat = WAVE_FORMAT_UNDEFINED;
    h->dwSampleLength = 0;
    h->exit = WAV_EXIT_SUCCESS;
}

/**
 * \fn load(char* filename, WavHeader* header, int** samples)
 * \author Brice Croix
 * \brief This functions loads the data in a .wav file in the wavHeader struct and samples array
 * \param[in] filename full path of the .wav to read.
 * \param[out] header address of the WavHeader struct where to store header data.
 * \param[out] samples address of the int array where  to store the samples, dynamically allocated. Converted into integers no matter what.
 * \return number of samples successfully loaded.
 */
__uint32_t load(char* filename, WavHeader* header, int** samples){
    __uint32_t nb_samples_read = 0;
    __uint8_t confirm_buffer;
    int32_t buffer; //All that matters is that it takes 4 bytes
    int success_check; //To check that the chunks were loaded properly

    if(header==NULL) return 0;

    initWavHeader(header);

    FILE* f = fopen(filename,"rb");
    if(f==NULL){
        fprintf(stderr,"Could not open the file %s, maybe it was mispelled.\n",filename);
        header->exit = WAV_EXIT_OPEN_ERROR;
        return 0;
    }

    fread(&buffer, sizeof(int32_t),1,f);
    if(buffer!=0x46464952){ //if the file doesn't start by "RIFF"
        fprintf(stderr,"This is not a wav file (not a RIFF file).\n");
        header->exit = WAV_EXIT_NOT_WAV;
        return 0;
    }

    fread(&(header->file_size), sizeof(__uint32_t),1,f);

    fread(&buffer, sizeof(int32_t),1,f);
    if(buffer!=0x45564157){ //if there is something else than "WAVE" here.
        fprintf(stderr,"This file's WAVE chunk is absent or at the wrong place.\n");
        header->exit = WAV_EXIT_NOT_PROPER_WAV;
        return 0;
    }



    do{
        //confirm_buffer is used to know if we have reached the end of the file.
        confirm_buffer = (__uint8_t)fread(&buffer, sizeof(int32_t),1,f);
        if(confirm_buffer!=0){
            switch (buffer) {
                case (0x20746d66): //Chunk id = "fmt "
                    success_check = loadFmtChunk(f, header);
                    if(success_check==EXIT_FAILURE) return 0;
                    break;
                case (0x74636166): //Chunk id = "fact"
                    success_check = loadFactChunk(f, header);
                    if(success_check==EXIT_FAILURE) return 0;
                    break;
                case (0x61746164): //Chunk id = "data"
                    nb_samples_read = loadDataChunk(f, header, samples);
                    break;
                case (0x7473696c): //Chunk id = "list", this chunk seems to correspond to metadata, indeed nothing to do here
                case (0x5453494c): //Chunk id = "LIST"
                case (0x20336469): //Chunk id = "id3 ", this chunk seems to correspond to metadata, indeed nothing to do here
                case (0x20334449): //Chunk id = "ID3 "
                case (0x4B4E554A): //Chunk id = "JUNK", this chunk is literally useless, it is meant to be skipped
                case (0x656B6146): //Chunk id = "Fake", I have absolutely no idea what is this chunk's purpose, only time I saw one it was filled with 0s
                    fread(&buffer, sizeof(int32_t),1,f);//The size of this chunk is indicated right after its tag, indeed let us go to its end.
                    fseek(f,buffer,SEEK_CUR);
                    break;
                default:
                    fprintf(stderr,
                            "An unknown chunk with tag '%c%c%c%c' has been detected in this file.\n",
                            (char) (buffer), (char) (buffer >> 8), (char) (buffer >> 16), (char) (buffer >> 24));
                    if(nb_samples_read!=0 && header->nSamplesPerSec!=0 && header->dwSampleLength!=0){
                        /*If all important data are already loaded, let us return*/
                        return nb_samples_read;
                    }else{
                        header->exit = WAV_EXIT_UNKNOWN_CHUNK;
                        /*The following lines are assuming that the file respects the RIFF norm, that is too risky*/
                        /*The size of a chunk is commonly indicated right after its tag, indeed let us go to its end.*/
                        //fread(&buffer, sizeof(int32_t),1,f);
                        //fseek(f,buffer,SEEK_CUR);
                        return 0;
                    }
            }
        }
    }while (confirm_buffer);
    fclose(f);

    if(nb_samples_read == 0) header->exit = WAV_EXIT_EMPTY;

    return nb_samples_read;
}

/**
 * \fn printWavHeader(WavHeader* h)
 * \author Brice Croix
 * \brief This functions prints on stdout the wavHeader data
 * \param h wavHeader to be printed
 */
void printWavHeader(WavHeader* h){
    if(h == NULL){
        printf("This WavHeader is empty (NULL).\n");
        return;
    }

    printf("--- printing the header ---\n\n");

    printf("This file size is %d bytes long (without counting the first 8).\n",h->file_size);

    printf("\nIn the fmt chunk:\n");
    switch(h->wFormatTag){
        case WAVE_FORMAT_UNDEFINED: printf("Its format is WAVE_FORMAT_UNDEFINED.\n"); break;
        case WAVE_FORMAT_PCM: printf("Its format is WAVE_FORMAT_PCM.\n"); break;
        case WAVE_FORMAT_ALAW: printf("Its format is WAVE_FORMAT_ALAW.\n"); break;
        case WAVE_FORMAT_MULAW: printf("Its format is WAVE_FORMAT_MULAW.\n"); break;
        case WAVE_FORMAT_IEEE_FLOAT: printf("Its format is WAVE_FORMAT_IEEE_FLOAT.\n"); break;
        case WAVE_FORMAT_EXTENSIBLE: printf("Its format is WAVE_FORMAT_EXTENSIBLE.\n"); break;
        default: printf("Its format is unknown.\n"); break;
    }
    printf("It has %d channels.\n",h->nChannels);
    printf("Must read %d samples per second.\n",h->nSamplesPerSec);
    printf("Indeed must read %d bytes per second.\n",h->nAvgBytesPerSec);
    printf("One data block (a sample of each channel) is %d bytes long.\n",h->nBlockAlign);
    printf("One sample of one channel is %d BITS long.\n",h->wBitsPerSample);

    printf("\nIf the file is not in PCM format:\n");
    printf("The fmt chunk has an extension of %d bytes.\n",h->cbSize);
    printf("Only %d bits are valid in each sample.\n",h->wValidBitsPerSample);
    printf("The speaker mapping has a mask : %x .\n",h->dwChannelMask);
    switch(h->subFormat){
        case WAVE_FORMAT_UNDEFINED: printf("The extension subformat is WAVE_FORMAT_UNDEFINED.\n"); break;
        case WAVE_FORMAT_PCM: printf("The extension subformat is WAVE_FORMAT_PCM.\n"); break;
        case WAVE_FORMAT_ALAW: printf("The extension subformat is WAVE_FORMAT_ALAW.\n"); break;
        case WAVE_FORMAT_MULAW: printf("The extension subformat is WAVE_FORMAT_MULAW.\n"); break;
        case WAVE_FORMAT_IEEE_FLOAT: printf("The extension subformat is WAVE_FORMAT_IEEE_FLOAT.\n"); break;
        case WAVE_FORMAT_EXTENSIBLE: printf("The extension subformat is WAVE_FORMAT_EXTENSIBLE.\n"); break;
        default: printf("The extension subformat is unknown.\n"); break;
    }

    printf("\nIn the fact chunk:\n");
    printf("There are %d samples per channel.\n",h->dwSampleLength);

    printf("\n--- End of header printing ---\n\n");

}

/**
 * \fn void* safe_malloc(size_t size)
 * \author Brice Croix
 * \brief This function is just a malloc with a featured security
 * \param size is the malloc parameter
 * \return same as malloc()
 */
void* safe_malloc(size_t __size){
    void* out;
    out = malloc(__size);
    if(out == NULL){
        fprintf(stderr,"Fatal : failed to allocate %ld bytes\n",__size);
        exit(EXIT_FAILURE);
    }
    return out;
}

/**
 * \fn void* safe_realloc(void* ptr, size_t size)
 * \author Brice Croix
 * \brief This function is just a realloc with a featured security
 * \param ptr is the first realloc parameter
 * \param size is the second realloc parameter
 * \return same as realloc()
 */
void* safe_realloc(void* ptr, size_t __size){
    void* ptr2;
    ptr2 = ptr;

    ptr = realloc(ptr,__size);
    if(ptr == NULL){
        fprintf(stderr,"Fatal : failed to allocate %ld bytes\n",__size);
        free(ptr2);
        exit(EXIT_FAILURE);
    }
    return ptr;

}

/**
 * \fn void* safe_calloc(size_t __nmenb, size_t __size)
 * \author Brice Croix
 * \brief This function is just a calloc with a featured security
 * \param ptr is the first calloc parameter
 * \param size is the second calloc parameter
 * \return same as calloc()
 */
void* safe_calloc(size_t __nmenb, size_t __size){
    void* out;
    out = calloc(__nmenb, __size);
    if(out == NULL){
        fprintf(stderr,"Fatal : failed to allocate %ld bytes\n",__size);
        exit(EXIT_FAILURE);
    }
    return out;

}
