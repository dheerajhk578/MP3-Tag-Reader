#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========= ANSI Escape Codes for Styling ========= 
#define GREEN   "\033[1;32m"   // Bright Green
#define RED     "\033[1;31m"   // Bright Red
#define YELLOW  "\033[1;33m"   // Bright Yellow
#define BLUE    "\033[1;34m"   // Bright Blue
#define CYAN    "\033[1;37m"   // Bright Cyan
#define MAGNETA   "\033[1;35m"   // Bright MAGNETA
#define RESET   "\033[0m"      // Reset to default color

// ========= Symbols for Pretty Output ========= 
#define CHECK   "✅"   // Success checkmark
#define CROSS   "❌"   // Error cross
#define MUSIC   "🎵"   // Music note
#define STAR    "✨"   // Star symbol


//Convert 4-byte big-endian to integer (used in ID3v2.3 frame sizes) 
unsigned int bigendian_to_int(unsigned char b[4])
{
    return (b[0]<<24) | (b[1]<<16) | (b[2]<<8) | b[3];
}

 
//Convert integer to 4-byte big-endian 
// (used when writing updated tag size back into the file) 
void int_to_big_endian(unsigned char b[4], unsigned int size)
{
    b[0] = (size >> 24) & 0xFF;
    b[1] = (size >> 16) & 0xFF;
    b[2] = (size >> 8) & 0xFF;
    b[3] = size & 0xFF;
}


  // Function to edit a specific MP3 metadata tag (like TIT2 for Title, TPE1 for Artist)
 // without affecting other tags or audio data.
 
void edit_mp3(const char *input_file, const char *target_tag, const char *new_value)
{
    // Section header 
    printf(CYAN "\n----------------------------------------------\n" RESET);
    printf(MAGNETA"  MODE SELECTED: EDIT METADATA\n" );
    printf(CYAN "----------------------------------------------\n\n" RESET);

    printf( "Validating tags...\n\n" );

    // Open input MP3 file in binary read mode 
    FILE *in = fopen(input_file, "r");
    if (in == NULL)
    { 
        printf(RED CROSS " Error opening input file\n" RESET); 
        return; 
    }
    printf(GREEN CHECK RESET" MP3 file opened successfully.\n");

    // Create temporary output file 
    FILE *out = fopen("temp.mp3", "w");
    if (out == NULL) 
    { 
        printf(RED CROSS " Error creating temp file\n" RESET); 
        fclose(in); 
        return; 
    }
    printf(GREEN CHECK RESET" Temporary output file created.\n");

    // Read and copy the 10-byte ID3 header 
    unsigned char header[10];
    if (fread(header, 1, 10, in) != 10) 
    { 
        printf(RED CROSS " Error reading header.\n" RESET); 
        fclose(in); fclose(out); return; 
    }
    fwrite(header, 1, 10, out);
    printf( GREEN CHECK RESET" Header copied successfully.\n" );

    int tag_found = 0; // Flag to check if target tag was found

    // Read frames (each frame = 4-byte tag + 4-byte size + 2-byte flags + data) 
    while (1)
    {
        unsigned char tag[4], size_bytes[4], flags[2];

        if (fread(tag, 1, 4, in) != 4)
        {
         break;       // EOF
        }
        if (fread(size_bytes, 1, 4, in) != 4) 
        {
            break;
        }
        if (fread(flags, 1, 2, in) != 2) 
        {
            break;
        }
        unsigned int size = bigendian_to_int(size_bytes);
        if (size == 0)
        {
           break; // No more frames
        }

        // Allocate memory for frame content 
        unsigned char *info = malloc(size);
        if (info == NULL) 
        {
           break;
        }

        if (fread(info, 1, size, in) != size) 
        { 
            free(info); 
            break; 
        }

        // Check if current frame is the target tag 
        if (strncasecmp((char*)tag, target_tag, 4) == 0)
        {
            // Calculate new size for updated value 
            unsigned int new_size = strlen(new_value) + 1; // +1 for encoding byte
            unsigned char new_size_bytes[4];
            int_to_big_endian(new_size_bytes, new_size);

            // Write updated frame 
            fwrite(tag, 1, 4, out);
            fwrite(new_size_bytes, 1, 4, out);
            fwrite(flags, 1, 2, out);

            unsigned char encoding = 0x00;  // Text encoding = ISO-8859-1
            fwrite(&encoding, 1, 1, out);
            
            fwrite(new_value, 1, strlen(new_value), out);

            tag_found = 1;
            printf(GREEN CHECK RESET" Edited tag: %s → %s\n", target_tag, new_value);
        }
        else
        {
            // Copy original frame without modification 
            fwrite(tag, 1, 4, out);
            fwrite(size_bytes, 1, 4, out);
            fwrite(flags, 1, 2, out);
            fwrite(info, 1, size, out);
        }

        free(info);
    }

    // Copy remaining audio data (after tags) 
    unsigned char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0)
    {
        fwrite(buffer, 1, bytes, out);
    }

    fclose(in);  //Close the original file
    fclose(out); // close the temp file

    // Replace original MP3 with updated one 
    if (remove(input_file) != 0)
    { 
        printf(RED CROSS " Error deleting original file\n" RESET);
    }
    else 
    {
        printf(GREEN CHECK RESET" Old file removed successfully.\n" );
    }

    if (rename("temp.mp3", input_file) != 0) 
    {
        printf(RED CROSS " Error renaming temp file\n" RESET);
    }
    else 
    {
        printf(GREEN CHECK RESET" New file renamed successfully.\n");
    }

    // Final status output 
    printf(CYAN "\n----------------------------------------------\n" RESET);

    if (tag_found)
    { 
        printf(MUSIC " " GREEN " MP3 TAG EDITING SUCCESSFUL! " RESET MUSIC "\n" 
               STAR " Your metadata has been updated! " STAR "\n");
    }
    else 
    {
        printf(RED CROSS"  \033[0mTag '%s' not found.\n" RESET, target_tag);
    }

    printf(CYAN "----------------------------------------------\n\n" RESET);
}