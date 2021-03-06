
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <sstream>

#include "Utilities.hh"

// removes leading and trailing whitespace from a string
std::string trim(const std::string& str, const std::string& whitespace ) {
    size_t strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos) {
        return "";
    }
    size_t strEnd = str.find_last_not_of(whitespace);
    size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

bool isInUserCode( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) {
    clang::FileID fid = ci.getSourceManager().getFileID(sl) ;
    bool ret = false ;
    if ( ! fid.isInvalid() ) {
        const clang::FileEntry * fe = ci.getSourceManager().getFileEntryForID(fid) ;
        if ( fe != NULL ) {
            char * resolved_path = almostRealPath( fe->getName() ) ;
            if ( resolved_path != NULL ) {
                if ( hsd.isPathInUserDir(resolved_path)) {
                    ret = true ;
                }
                free(resolved_path) ;
            }
        }
    }
    return ret ;
}

bool isInUserOrTrickCode( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) {
    clang::FileID fid = ci.getSourceManager().getFileID(sl) ;
    bool ret = false ;
    if ( ! fid.isInvalid() ) {
        const clang::FileEntry * fe = ci.getSourceManager().getFileEntryForID(fid) ;
        if ( fe != NULL ) {
            char * resolved_path = almostRealPath( fe->getName() ) ;
            if ( resolved_path != NULL ) {
                if ( hsd.isPathInUserOrTrickDir(resolved_path)) {
                    ret = true ;
                }
                free(resolved_path) ;
            }
        }
    }
    return ret ;
}

std::string getFileName( clang::CompilerInstance & ci , clang::SourceLocation sl , HeaderSearchDirs & hsd ) {
    clang::FileID fid = ci.getSourceManager().getFileID(sl) ;
    if ( ! fid.isInvalid() ) {
        const clang::FileEntry * fe = ci.getSourceManager().getFileEntryForID(fid) ;
        if ( fe != NULL ) {
            char * resolved_path = almostRealPath( fe->getName() ) ;
            if ( resolved_path != NULL  and hsd.isPathInUserDir(resolved_path)) {
                return std::string(resolved_path) ;
            }
        }
    }
    return std::string() ;
}

#include <iostream>
char * almostRealPath( const std::string& in_path ) {
    return almostRealPath(in_path.c_str());
}

/*
   This function returns almost absolute path of the directory of the incoming path.
   It does not follow the last symbolic link.

   dirname and basename may modify their argument, so make a copy of the path.
*/
char * almostRealPath( const char * in_path ) {
    char * file_copy_path = strdup(in_path) ;
    char * dir_copy_path = strdup(in_path) ;
    char * file = basename(file_copy_path) ;
    char * dir = dirname(dir_copy_path) ;
    char * resolved_path = NULL ;
    char * final_path = NULL ;
    resolved_path = realpath( dir , resolved_path) ;
    if ( resolved_path != NULL ) {
        final_path = (char *)malloc(strlen(resolved_path) + strlen(file) + 2) ;
        sprintf(final_path,"%s/%s", resolved_path , file ) ;
        free(resolved_path) ;
    }
    free(file_copy_path) ;
    free(dir_copy_path) ;
    return final_path ;
}

static const std::string escapeSequence = "[";
static const std::string defaultForegroundColorSequence = escapeSequence + "39m";
static const std::string boldSequence = escapeSequence + "1m";
static const std::string noBoldSequence = escapeSequence + "21m";
static const std::string underlineSequence = escapeSequence + "4m";
static const std::string noUnderlineSequence = escapeSequence + "24m";

std::string color(const Color& color, const std::string& text) {
    std::ostringstream oss;
    oss << escapeSequence << color << "m" << text << defaultForegroundColorSequence;
    return oss.str();
}

std::string bold(const std::string& text) {
    return boldSequence + text + noBoldSequence;
}

std::string underline(const std::string& text) {
    return underlineSequence + text + noUnderlineSequence;
}

std::string underline(const std::string& text, unsigned length) {
    return underline(text.substr(0, length)) + text.substr(length);
}

std::string quote(const std::string& text) {
    return "\"" + text + "\"";
}
