/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <string>
#include <fstream>
#include <vector>

#include <iostream>
#include <sstream>
using namespace std;
//#include <stdlib.h>

/*
#include "WavFile.h"
#include "SoundTouch.h"
using namespace soundtouch;
*/
#include "WavSet.h"


#include <ctime>

int main(int argc, char *argv[])
{
	///////////////////
	//read in arguments
	///////////////////
	double fMaxSecondsPerWavFile = 30;
	//double fSecondsPerSegment = 1.0;
	double fSecondsPerSegment = 0.020; //20ms
	//float fSecondsPlay = -1; //play once
	float fSecondsPlay = 0.5;
	bool boolRandomNavigation = 0; //0=sequentialy or 1=randomly
	if(argc>1)
	{
		//first argument is the maximum wav file length in seconds, i.e. longer wav files shall be ignored
		fMaxSecondsPerWavFile = atof(argv[1]);
	}
	if(argc>2)
	{
		//second argument is the segment length in seconds
		fSecondsPerSegment = atof(argv[2]);
	}
	if(argc>3)
	{
		//third argument is the time it will shuffle
		fSecondsPlay = atof(argv[3]);
	}
	if(argc>4)
	{
		//fourth argument defines how to browse through filenames, 0=sequentialy or 1=randomly?
		boolRandomNavigation = atoi(argv[4]);
	}

	/////////////////////////////////////////////
	//execute cmd line to get all sub foldernames
	/////////////////////////////////////////////
	system("DIR *. /S /B /O:N > foldernames.txt");
	/*
	system("DIR *.wav /B /O:N > filenames.txt");
	*/

	/////////////////////////////////////////
	//load foldernames.txt into string vector
	/////////////////////////////////////////
	vector<string> foldernames;
	ifstream ifs("foldernames.txt");
	string temp;
	while(getline(ifs,temp))
		foldernames.push_back(temp);

	/*
	////////////////////////////////////////////
	//load .wav filenames.txt into string vector
	////////////////////////////////////////////
	vector<string> filenames;
	ifstream ifs("filenames.txt");
	string temp;
	while(getline(ifs,temp))
		filenames.push_back(temp);
	*/

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//browse throught each folder's filenames, filter out folders containing too large .wav files and call spisplitpatternplay.exe
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	double fSecondsPerWavFile = -1.0;
	string quote;
	quote = "\"";
	string cmdbuffer;
	string param1;
	string param2;
	stringstream ss1 (stringstream::in | stringstream::out);
	stringstream ss2 (stringstream::in | stringstream::out);
	ss1 << fSecondsPerSegment;
	param1 = ss1.str();
	ss2 << fSecondsPlay;
	param2 = ss2.str();
	vector<string>::iterator it;
	vector<string>::iterator iterator;
	cout << "foldernames.txt contains:" << endl << endl;


	//////////////////////////
	//initialize random number
	//////////////////////////
	srand((unsigned)time(0));
	int random_integer=-1;
	int lowest=1, highest=foldernames.size();
	int range=(highest-lowest)+1;
	//random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
	int random_integer2=-1;
	int lowest2=1, highest2=foldernames.size();
	int range2=(highest2-lowest2)+1;
	//random_integer2 = lowest2+int(range2*rand()/(RAND_MAX + 1.0));

	it=foldernames.begin();
	while(1)
	//for ( it=foldernames.begin() ; it < foldernames.end(); it++ )
	{
		cout << *it << endl; //*it is a foldername

		////////////////////////////////////////////////////////
		//execute cmd line to get all subfolder's .wav filenames
		////////////////////////////////////////////////////////
		string pathfilter;
		pathfilter = *it + "\\*.wav";
		string systemcommand;
		systemcommand = "DIR " + quote + pathfilter + quote + "/B /O:N > filenames.txt";
		//system("DIR *.wav /B /O:N > filenames.txt");
		system(systemcommand.c_str());

		////////////////////////////////////////////
		//load .wav filenames.txt into string vector
		////////////////////////////////////////////
		vector<string> filenames;
		ifstream ifs("filenames.txt");
		string temp;
		while(getline(ifs,temp))
		filenames.push_back(temp);

		bool skip=true; //will skip if there is no .wav file
		string fullfilename;
		string fullfilenames;
		int i=0;
		for ( iterator=filenames.begin() ; iterator < filenames.end(); iterator++ )
		{
			skip=false; //there is at least one .wav file
			/*
			WavInFile* pWavInFile = NULL;
			try
			{
				fullfilename = *it + "\\" + *iterator;
				pWavInFile = new WavInFile(fullfilename.c_str());
			}
			catch(char* str) 
			{
				cout << "Exception raised within new WavInFile(): " << str << '\n';
			}
			if(pWavInFile)
			*/
			/////////////////
			//read a WAV file 
			/////////////////
			fullfilename = *it + "\\" + *iterator;
			WavSet* pWavSet = new WavSet;
			pWavSet->ReadWavFileHeader(fullfilename.c_str());
			if(pWavSet)
			{
				fSecondsPerWavFile = pWavSet->totalFrames/(1.0*pWavSet->SampleRate); //pWavInFile->getNumSamples()/pWavInFile->getSampleRate();
				int numChannels = pWavSet->numChannels;//pWavInFile->getNumChannels();
				/*
				delete pWavInFile;
				*/
				delete pWavSet;

				//if(fSecondsPerWavFile > fMaxSecondsPerWavFile || numChannels!=2)
				if(fSecondsPerWavFile > fMaxSecondsPerWavFile)
				{
					cout << "skipping folder " << *it << endl;
					skip=true; //will skip if one .wav file is found to be invalid
					break;
				}
				#ifdef _DEBUG
				if(0)
				{
					fullfilenames = fullfilenames + " " + quote + fullfilename + quote;
					/*
					//everytime
					i=i+1;
					if(1)
					*/
					/*
					//once every 5 times
					i=i+1;
					if(i%5==0)
					*/
					if(i==0)
					{
						//cmdbuffer = "spiplay.exe " + quote + fullfilename + quote + " " + param1 + " " + param2; 
						cmdbuffer = "spiplay.exe " + quote + fullfilename + quote + " " + param2; 
						cout << cmdbuffer << endl;
						system(cmdbuffer.c_str());
						if(0)
						{
							//trying to play simultaneously multiple files, todo: as in parallel
							cmdbuffer = "sox.exe --combine sequence " + fullfilenames + " -d"; 
							cout << cmdbuffer << endl;
							system(cmdbuffer.c_str());
							fullfilenames = "";
						}
						i=random_integer2 = lowest2+int(range2*rand()/(RAND_MAX + 1.0));
					}
					i=i-1;
				}
				#endif //_DEBUG
			}
			else
			{
				cout << "skipping folder " << *it << "(new WavInFile() returns NULL)" << endl;
				skip=true; //will skip if one .wav file cannot be opened using WavInFile object
				break;
			}
		}

		if(skip==false)
		{
			//cmdbuffer = "C:\\spoirier\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_spi\\spisplitpatternplay\\Release\\spisplitpatternplay.exe " + quote + *it + quote + " " + param1 + " " + param2; //*it is a foldername
			cmdbuffer = "C:\\spoirier\\oifii-org\\httpdocs\\ns-org\\nsd\\ar\\cp\\audio_spi\\spisplitpatternplay\\Release\\spisplitpatternplay.exe " + quote + *it + quote + " " + param2 + " " + param1; //*it is a foldername
			cout << cmdbuffer << endl;
			system(cmdbuffer.c_str()); 
		}
		else
		{
			cout << "folder has been skipped." << endl << endl;
		}

		//sequential access or random access
		if(boolRandomNavigation==0)
		{
			//sequential
			it++;
			if(it==foldernames.end())
			{
				it=foldernames.begin();
			}
		}
		else
		{
			//random
			random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
			it = foldernames.begin() + random_integer-1;
		}
	}
	cout << endl;


	
	return 0;
}