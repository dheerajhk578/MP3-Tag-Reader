/* 
   PROJECT_TITLE : MP3 Tag Reader and Editor
   AUTHOR        : DHEERAJ H K
   DATE          : 18 / 11 / 2025
   DESCRIPTION   :
   -----------------------------------------
   A C-based console application that allows users to 
   read, display, and edit ID3v1 metadata tags from MP3 files.

   The MP3 Tag Reader and Editor is a C-based console application that allows users to read, view, and edit ID3 tags in MP3 audio files. It supports ID3v1, ID3v2, and ID3v2.3 versions, making it compatible with most MP3 songs.
   The program lets users display and modify details such as Title, Artist, Album, Year, Comment, and Genre using a simple menu-driven interface. It automatically detects the tag version and performs the necessary read or write operations safely.

  ->This project demonstrates the use of file handling, structures, and binary data manipulation in C, and serves as a good learning project for understanding how metadata is stored and processed in audio files.

    KEY FEATURES

    - Supports ID3v1, ID3v2, and ID3v2.3 tag formats.
    - Reads, displays, and edits song information.
    - Simple menu-based interface for easy use.
    - Automatically detects the tag version.
    - Safe file operations with error handling.
    - Compact and efficient C code.

    APPLICATIONS

    - Used to organize and edit MP3 song information.
    - Helps students learn file handling and binary I/O in C.
    - Can be extended to support more versions or features in the future.
    */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

// Function declarations
void read_Mp3_tag(FILE *fp);   
void edit_mp3(const char *input_file, const char *target_tag, const char *new_value);

// Function to print usage guide
void print_usage(const char *prog_name)
{
    printf(RED "\n❌ ERROR: INVALID ARGUMENT\n\n" RESET);
    printf("------------------------------------\n");
    printf(YELLOW "         USAGE GUIDE             \n" RESET);
    printf("-------------------------------------\n");
    printf("  %s " BLUE " -v <filename>" RESET "                      -> View tags\n", prog_name);
    printf("  %s " BLUE" -e <tag> <new_value> <filename>" RESET "    -> Edit a tag\n", prog_name);
    printf("  %s " BLUE " --help" RESET "                             -> Show help menu\n\n", prog_name);
}

// Function to print help menu
void print_help()
{
    printf(CYAN "\n--------------------📑 HELP MENU--------------------------\n" RESET);
    printf(YELLOW "OPTIONS:\n" RESET);
    printf("  A." GREEN "  -v <filename>" RESET "           -> To View MP3 file metadata\n");
    printf("  B." GREEN "  -e <tag> <value> <file>" RESET " -> TO Edit MP3 file metadata\n");
    printf(CYAN "-----------------------------------------------------------\n" RESET);


    printf(YELLOW "EDIT TAG OPTIONS:\n" RESET);
    printf("   C.1 " MAGENTA "TIT2" RESET "  -> TO Edit Song Title\n");
    printf("   C.2 " MAGENTA "TPE1" RESET "  -> TO Edit Artist Name\n");
    printf("   C.3 " MAGENTA "TALB" RESET "  -> TO Edit Album Name\n");
    printf("   C.4 " MAGENTA "TYER" RESET "  -> TO Edit Year\n");
    printf("   C.5 " MAGENTA "TCON" RESET "  -> TO Edit Genre\n");
    printf("   C.6 " MAGENTA "COMM" RESET "  -> TO Edit Comment\n");

    printf(CYAN "------------------------------------------------------\n\n" RESET);
}

int main(int argc, char *argv[])
{
    FILE *fp;

    // Case 1: No arguments
    if (argc < 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    // Case 2: Help option
    if (strcmp(argv[1], "--help") == 0)
    {
        print_help();
        return 0;
    }

    // Case 3: VIEW mode
    if (strcasecmp(argv[1], "-v") == 0)
    {
        if (argc < 3)
        {
            printf(RED "Error: Missing filename for view mode.\n" RESET);
            print_usage(argv[0]);
            return 1;
        }

        // Open file
        fp = fopen(argv[2], "r");
        if (fp == NULL)
        {
            perror(RED "Error opening file" RESET);
            return 1;
        }

        read_Mp3_tag(fp); // Call function
        fclose(fp);       // Close file
    }
    // Case 4: EDIT mode
    else if (strcasecmp(argv[1], "-e") == 0)
    {
        if (argc < 5)
        {
            printf(RED "Error: Edit mode requires 3 arguments: <tag> <new_value> <filename>\n" RESET);
            print_usage(argv[0]);
            return 1;
        }

        const char *target_tag = argv[2];
        const char *new_value  = argv[3];
        const char *input_file = argv[4];

        edit_mp3(input_file, target_tag, new_value); // Call edit function
    }
    // Case 5: Invalid option
    else
    {
        printf(RED "Invalid option. Use -v for VIEW, -e for EDIT, or --help for help\n" RESET);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}