#include <cstdio>
#include <Parameters.h>

#include "DBReader.h"
#include "Debug.h"
#include "Util.h"


int createfasta (int argc, const char * argv[])
{
    std::string usage("Converts a ffindex database to fasta \n");
    usage.append("Written by Martin Steinegger (Martin.Steinegger@campus.lmu.de) & Maria Hauser (mhauser@genzentrum.lmu.de).\n\n");
    usage.append("USAGE: <queryDB> <targetDB> <resultDB> <fastaDB>\n");
    Parameters par;
    par.parseParameters(argc, argv, usage, par.onlyverbosity, 4);

    Debug(Debug::WARNING) << "Query file is " <<  par.db1 << "\n";
    std::string queryHeaderDB =  par.db1 + "_h";
    DBReader querydb_header(queryHeaderDB.c_str(), std::string(queryHeaderDB+".index").c_str());
    querydb_header.open(DBReader::NOSORT);

    Debug(Debug::WARNING) << "Target file is " << par.db2 << "\n";
    std::string targetHeaderDB =  par.db2 + "_h";
    DBReader targetdb_header(targetHeaderDB.c_str(), std::string(targetHeaderDB+".index").c_str());
    targetdb_header.open(DBReader::NOSORT);

    Debug(Debug::WARNING) << "Data file is " << par.db3 << "\n";
    DBReader dbr_data( par.db3.c_str(), std::string( par.db3+".index").c_str());
    dbr_data.open(DBReader::NOSORT);

    FILE *fastaFP =  fopen(par.db4.c_str(), "w");
    char header_start[] = {'>'};
    char newline[] = {'\n'};
    Debug(Debug::WARNING) << "Start writing file to " << par.db4 << "\n";
    char * dbKey = new char[par.maxSeqLen];
    for(size_t i = 0; i < dbr_data.getSize(); i++){
        
        fwrite(header_start, sizeof(char), 1, fastaFP);
        const char * key = dbr_data.getDbKey(i).c_str();

        char * header_data = querydb_header.getDataByDBKey(key);
        fwrite(header_data, sizeof(char), strlen(header_data) - 1, fastaFP);

        fwrite(newline, sizeof(char), 1, fastaFP);
        // write data
        char * data = dbr_data.getData(i);
        while(*data != '\0') {
            Util::parseKey(data, dbKey);
            char * header_data = targetdb_header.getDataByDBKey(key);
            std::string dbkey = Util::parseFastaHeader(header_data);
            fwrite(dbkey.c_str(), sizeof(char), dbkey.length(), fastaFP);
            data = Util::skipLine(data);
        }
    }
    delete [] dbKey;
    Debug(Debug::WARNING) << "Done." << "\n";

    fclose(fastaFP);
    targetdb_header.close();
    querydb_header.close();
    dbr_data.close();


    return 0;
}
