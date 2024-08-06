

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


const vector<byte> bunksig = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

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

	out << img.rdbuf();

	for(char c : bunksig){
		out.put(c);
	}

	out << exec.rdbuf();

	std::cout << "bunk: buffers dumped into " << outp << "\n";

	img.close();
	exec.close();
	out.close();

	std::cout << "bunk: binaries closed\nbunk: out file at " << outp << std::endl;
}

void extract(const std::string& bunkp, const std::string& outp) {
    std::ifstream bnk(bunkp, std::ios::binary);
    if (!bnk.is_open()) {
        std::cout << "bunk: source img crash" << std::endl;
        return;
    }

    std::ofstream out(outp, std::ios::binary);
    if (!out.is_open()) {
        std::cout << "bunk: target exe crash" << std::endl;
        return;
    }

    const std::size_t nbytes = 2000000; // 2 MB max
    std::vector<char> buf(nbytes);

    if (bnk.read(buf.data(), buf.size()) || bnk.gcount() > 0) {
        const auto nread = bnk.gcount();
        std::cout << "bunk: read " << nread << " bytes from input " << bunkp << "\n";

        std::vector<byte> bytes(buf.begin(), buf.begin() + nread);

        auto it = std::search(bytes.begin(), bytes.end(), bunksig.begin(), bunksig.end());

        if (it != bytes.end()) {
            int idx = std::distance(bytes.begin(), it) + bunksig.size();
            std::cout << "bunk: signature found at byte " << std::distance(bytes.begin(), it) << ", dumping bytes to " << outp << "\n";

            out.write(reinterpret_cast<char*>(&bytes[idx]), nread - idx);

            while (bnk.read(buf.data(), buf.size())) { // i hate buffers basically this is in case it would overflow above
                out.write(buf.data(), buf.size());
            }

            out.write(buf.data(), bnk.gcount()); // remaining bytes. see above

            std::cout << "bunk: bytes dumped into " << outp << "\n";
        } else {
            std::cout << "bunk: failed to find signature" << std::endl;
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

	if(splargs[0]=="hide")
		dump(splargs[1], splargs[2], splargs[3]);
	
	else if(splargs[0]=="find")
		extract(splargs[1], splargs[2]);
	
}


/* e.exe contents
#include<stdio.h>

int main(){
    printf("evil grad photo\n");
    return 0;
}
*/