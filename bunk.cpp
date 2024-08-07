

#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include<algorithm>


using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;

typedef unsigned char byte;

/* compressing the image would be best, as it would give a smaller file which would be less eyecatching*/

vector<byte> generatesignature(int s){
	srand(s);
	vector<byte> ret;
	int bytelength = (rand() % 224) + 32;
	for(int i=0; i<bytelength; i++){
		int r = rand() % 255;
		ret.push_back(r);
	}

	return ret;
}

void dump(const string& execp, const string& imgp, const string& outp){

	ifstream img(imgp, std::ios::binary);

	if(!img.is_open()){
		std::cout << "bunk: source img crash";
		return;
	}

	ifstream exec(execp, std::ios::binary);

	if(!exec.is_open()){
		std::cout << "bunk: source exe crash";
		return;
	}

	ofstream out(outp, std::ios::binary);

	if(!out.is_open()){
		std::cout << "bunk: target img crash";
		return;
	}

	std::cout << "bunk: binaries open\n";

	// first
	out << img.rdbuf();

	// sig
	unsigned long sfi = INT_FAST32_MIN;
	img.clear();
	img.seekg(1024);

	img.read(reinterpret_cast<char *>(&sfi), sizeof(sfi));

	if(sfi == INT_FAST32_MIN){
		std::cout << "bunk: failed getting seed from image\n";
		return;
	}

	std::cout << "bunk: image seed given as " << sfi << "\n";
	
	vector<byte> bunksig = generatesignature(sfi);

	for(char c : bunksig){
		out.put(c);
	}

	// second
	out << exec.rdbuf();

	std::cout << "bunk: buffers dumped into " << outp << "\n";

	img.close();
	exec.close();
	out.close();

	std::cout << "bunk: binaries closed\nbunk: out file at " << outp << std::endl;
}

void extract(const std::string& bunkp, const std::string& outp) {
    std::ifstream bnk(bunkp, std::ios::binary);
    if(!bnk.is_open()){
        std::cout << "bunk: source img crash" << std::endl;
        return;
    }

    std::ofstream out(outp, std::ios::binary);
    if(!out.is_open()){
        std::cout << "bunk: target exe crash" << std::endl;
        return;
    }

    const std::size_t nbytes = 2000000; // 2mb in buff
    std::vector<char> buf(nbytes);

	unsigned long sfi = INT_FAST32_MIN;

	bnk.clear();
	bnk.seekg(1024);
	bnk.read(reinterpret_cast<char*>(&sfi), sizeof(sfi));

	if(sfi == INT_FAST32_MIN){
		std::cout << "bunk: failed getting seed from image\n";
		return;
	}

	std::cout << "bunk: image seed given as " << sfi << "\n";

	vector<byte> bunksig = generatesignature(sfi);

	bnk.clear();
	bnk.seekg(0, std::ios::beg);

    if(bnk.read(buf.data(), buf.size()) || bnk.gcount() > 0){
        const auto nread = bnk.gcount();
        std::cout << "bunk: read " << nread << " bytes from input " << bunkp << "\n";

        std::vector<byte> bytes(buf.begin(), buf.begin() + nread);

        auto it = std::search(bytes.begin(), bytes.end(), bunksig.begin(), bunksig.end());

        if(it != bytes.end()){
            int idx = std::distance(bytes.begin(), it) + bunksig.size();
            std::cout << "bunk: signature found at byte " << std::distance(bytes.begin(), it) << ", dumping bytes to " << outp << "\n";

            out.write(reinterpret_cast<char*>(&bytes[idx]), nread - idx);

            while (bnk.read(buf.data(), buf.size())) { // i hate buffers!
                out.write(buf.data(), buf.size());
				std::cout << "bunk: read " << buf.size() << " bytes from input " << bunkp << "\n";
            }

            out.write(buf.data(), bnk.gcount()); // see above

            std::cout << "bunk: bytes dumped into " << outp << "\n";
        }else{
            std::cout << "bunk: failed to find signature" << std::endl;
			return;
        }

        bnk.close();
        out.close();

        std::cout << "bunk: out file at " << outp << std::endl;
    } else {
        std::cout << "bunk: failed to read input " << bunkp << std::endl;
    }
}

vector<string> tokenize(string s){
	std::stringstream ss(s);
	vector<string> v;
	string w;
	while(ss >> w){
		v.push_back(w);
	}

	return v;
}


int main(){

	string args;
	std::getline(std::cin, args);

	vector<string> splargs = tokenize(args);

	if(splargs[0]=="exit")
		return 0;

	else if(splargs[0]=="hide")
		dump(splargs[1], splargs[2], splargs[3]);
	
	else if(splargs[0]=="find")
		extract(splargs[1], splargs[2]);
	
}