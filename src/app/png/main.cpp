#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <math.h>
#include <modtoollib/modtool.h>

using namespace std;

time_t get_last_modified( char const* path )
{
    struct stat info;
    int result = stat( path, &info );
    if( result == 0 )
    {
        return info.st_mtime;
    }
    else
    {
        return 0;
    }
    
}

bool is_more_recent( char const* path_a, char const* path_b )
{
    time_t a_time = get_last_modified( path_a );
    time_t b_time = get_last_modified( path_b );
    return a_time < b_time;
}

bool exists( char const* path )
{
	struct _stat info;
	int result = _stat( path, &info );
	return result == 0;
}

char const* get_file_name( char const* path )
{
	char const* name = strrchr( path, '/' );
	if( !name )
	{
		return path;		
	}
	return name;
}

void get_output_file_path( char const* input_file_path, char* output_file_path )
{
    strcpy( output_file_path, input_file_path );
    strcpy( strrchr( output_file_path, '.' ), ".xml" );
}

int main( int argument_count, char** arguments )
{
    set_application_folder( arguments[0] );
	set_asset_name( "" );
    begin_log();

	if( argument_count != 3 )
	{
		error( "ERROR: Invalid number of arguments!\n" );
	}

	char const* input_file_path = arguments[1];

	if( !exists( input_file_path ) )
	{
		error( "ERROR: Could not open '%s'!\n", input_file_path );
	}

    char input_folder[1024];
    get_folder( input_file_path, input_folder );

    char output_package_file_path[MAX_PATH_LEN];
    get_output_file_path( input_file_path, output_package_file_path );

	char build_tool_path[1024];
	sprintf(build_tool_path, "%scompiler_scripts/image_build.py", get_application_folder());

    if( is_more_recent( input_file_path, output_package_file_path ) && is_more_recent( arguments[0], output_package_file_path ) && is_more_recent( build_tool_path, output_package_file_path  ) )
    {
        return 0;
    }

    std::vector< char const* > image_paths;

	char command_line[4096];

    char const* output_dir = "data";
    if( strstr( input_folder, "mods" ) )
    {
        output_dir = ".";
    }

    sprintf( command_line, "%s\\buildtools\\windows\\Python27\\python.exe \"%s\" \"%s\"", get_application_folder(), build_tool_path, input_file_path);
    
    run( command_line, true, "Compiling '%s'", input_file_path );

    end_log();

	return 0;
}
