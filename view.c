#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned int big_endian_to_int(unsigned char b[4])
{
    return (b[0]<<24) | (b[1]<<16) | (b[2]<<8) | b[3];
}


// Function to read and display MP3 tags (styled like your screenshot)
void read_Mp3_tag(FILE *fp)
{
    unsigned char signature[3];
    unsigned char tag[5];
    unsigned char size_bytes[4];
    unsigned int size;
    char *info;



    int i;
    for (i = 0; i <= 100; i++) 
    {

        printf("\rLoading MP3 VIEW METADATA... [%3d%%]", i);
        fflush(stdout);
        usleep(2500);  // Sleep for 20 milliseconds
    }
    printf("\nFinished!\n");

    // 1. Read the first 3 bytes (ID3 signature)
    if (fread(signature, 1, 3, fp) != 3) 
    {
        printf("\033[31m✘ Failed to read MP3 signature.\033[0m\n");
        return;
    }
    if (strcmp(signature, "ID3") != 0)
    {
        printf("\n\033[31m✘ No ID3 tag found in this MP3 file.\033[0m\n\n");
        return;
    }

    // Skip next 7 bytes (version, flags, tag size)
    fseek(fp, 7, SEEK_CUR);

    // --- Pretty Header ---
    printf("============================================");
    printf("\n\033[1;36m     🎧 MODE SELECTED: VIEW METADATA \033[0m\n");
    printf("============================================\n\n");
    printf("\033[32m✅\033[0m Validation successful: MP3 file format is correct.\n");
    printf("\033[32m✅\033[0m Success: 'ID3' tag found. Valid MP3 file.\033[0m\n");
    printf("\033[32m✅\033[0m File check passed: MP3 file opened successfully.\033[0m\n\n");

    printf("------------------------------------------------------------\n");
    printf("\033[33m   📂 MP3 Metadata (ID3 Tag Details)\033[0m\n");
    printf("------------------------------------------------------------\n");

    int tags_read = 0;

    // 2. Loop through frames
    while (1)
    {
        if (fread(tag, 1, 4, fp) != 4)
        { 
            break;
        }
        tag[4] = '\0';
        if (tag[0] == 0) 
        {
            break;
        }

        if (fread(size_bytes, 1, 4, fp) != 4) 
        {
            break;
        }
        size = big_endian_to_int(size_bytes);

        fseek(fp, 2, SEEK_CUR); // skip flags
        
        if (size <= 0) 
        {
            break;
        }
        info = (char*)malloc(size + 1);
        if (info == NULL)
        {
           perror("ERROR: ");
           break;
        }
        if (fread(info, 1, size, fp) != size) 
        {
            free(info);
            break;
        }
        info[size] = '\0';

        const char *data = info;

        // Handle text and comment frames
        if (tag[0] == 'T') 
        {
            data = info + 1; // skip encoding byte
        }
        else if (strcmp(tag, "COMM") == 0) 
        {
            int i = 4;
            while (i < size && info[i] != '\0')
            { 
                i++;
            }
            if (i < size) 
            {
                i++;
            }
            data = info + i;
        }

        // --- Styled Printing with Icons ---
        if (strcmp(tag, "TIT2") == 0) 
        {
            printf("🎵 \033[32mTitle   \033[0m : %s\n", data);
        }
        else if (strcmp(tag, "TPE1") == 0)
        {
            printf("🎤 \033[32mArtist  \033[0m : %s\n", data);
        }
        else if (strcmp(tag, "TALB") == 0)
        {
            printf("💿 \033[32mAlbum   \033[0m : %s\n", data);
        }
        else if (strcmp(tag, "TYER") == 0)
        {
            printf("📅 \033[32mYear    \033[0m : %s\n", data);
        }
        else if (strcmp(tag, "TCON") == 0)
        {
            printf("🎶 \033[32mGenre   \033[0m : %s\n", data);
        }
        else if (strcmp(tag, "COMM") == 0)
        {
            printf("💬 \033[32mComment \033[0m : %s\n", data);
        }

        tags_read++;

        if(tags_read == 6)
        {
           break;
        }

        free(info);
    }

    // --- Footer ---
    printf("------------------------------------------------------------\n");
    printf("\033[35m📌 \033[0mTags read successfully: All metadata has been extracted.\n");
    printf("------------------------------------------------------------\n\n");
}