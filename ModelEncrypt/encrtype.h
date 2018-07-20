int loadWeights(char* path, map<string, float*> &weights)
{
	FILE *loadData;
	loadData = fopen(path, "rb");
	int* size = (int*)malloc(1 * sizeof(int));
	float* weight;
	int rc = 1;
	char* name = (char*)malloc(0 * sizeof(char));

	char buffer[1200500];//内存池

	long long index = 0;
	int length = Key.length();

	while (rc != 0)
	{
//		rc = fread(size, sizeof(int), 1, loadData);        //read the length of name
		rc = fread(buffer, sizeof(char), 1*sizeof(int)/sizeof(char), loadData);        //read the length of name
		
		if( rc< sizeof(int)/sizeof(char)) break;

		for(int i=0;i<1 * sizeof(int)/sizeof(char);i++ ){
			buffer[i]^=static_cast<char>(Key[index % length]);index++ ;
		}
		std::memcpy(size,buffer,1*sizeof(int)/sizeof(char));
	

		if (rc == 0)
		{
			break;
		}

		free(name);

		name = (char*)malloc((1 + size[0])*sizeof(char));

//		rc = fread(name, sizeof(char), size[0], loadData); //read name

		rc = fread(buffer, sizeof(char), size[0], loadData); //read name
		for(int i=0; i<size[0]; i++){
			buffer[i]^=static_cast<char>(Key[index % length]);index++ ;
		}
		std::memcpy(name,buffer,size[0]);


		name[size[0]] = '\0';
		string s = name;

		//rc = fread(size, sizeof(int), 1, loadData);        //read the length of weight
		rc = fread(buffer, sizeof(char), 1*sizeof(int)/sizeof(char), loadData);        //read the length of name

		for(int i=0;i<1 * sizeof(int)/sizeof(char);i++ ){
			buffer[i]^=static_cast<char>(Key[index % length]);index++ ;
		}
		std::memcpy(size,buffer,1*sizeof(int)/sizeof(char));


		weights[s] = (float*)malloc(size[0] * sizeof(float));

		
		//rc = fread(weights[s], sizeof(float), size[0], loadData);//read weight
		rc = fread(buffer, sizeof(char), size[0]*sizeof(float)/sizeof(char), loadData);//read weight

		for(int i=0;i<size[0]*sizeof(float)/sizeof(char);i++) {
			buffer[i]^=static_cast<char>(Key[index % length]);index++ ;
		}
		

		for(int i=0;i<size[0];i++) {
			std::memcpy(&weights[s][i],&buffer[sizeof(float) * i],sizeof(float));
		}
	}
	return rc;
}

void Encrypt(const std::string input_filename,const std::string output_filename,const std::string Key)
{
	
	std::ifstream ifs;
	ifs.open(input_filename,std::ios::in|std::ios::binary);


	std::ofstream ofs;
	ofs.open(output_filename,std::ios::out|std::ios::binary);

	char ch;

	long long  index = 0;
	int length = Key.length();

	while(!ifs.eof())
	{
		ifs.read(&ch,1);
		ofs<< static_cast<char>((ch^( char )Key[index % length]));
		++index;
	}

	ifs.close();
	ofs.close();
}
