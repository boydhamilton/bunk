

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
using std::cout;


typedef unsigned char byte;


/*
https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher
*/


vector<byte> signature(unsigned int s){
	srand(s);
	vector<byte> ret;
	int bytelength = (rand() % 224) + 32;
	for(int i=0; i<bytelength; i++){
		int r = rand() % 255;
		ret.push_back(r);
	}

	return ret;
}

	// 1byte to 1byte, future could do 1byte to nbytes to further encrypt, bingbong problem III esque

	/*
	design
		returns a vector of bytes that contains the vigenere key seeded by the given input image

		go through each 8bit permutation (0-255 gives 256 distinct values)
		for each one choose a random byte from a pre-initialized array
			remove that value afterwards

		should work for basic 1-1 byte vigenere cipher design
	*/


vector<byte> vigenerekey(unsigned int s){

	srand(s);

	vector<byte> k;
	vector<byte> r;

	for(int i=0; i<256; i++)
		r.push_back(static_cast<byte>(i));


	for(int i=0; i<256; i++){

		if(r.empty()){
			cout << "bunk: reference vector empty at iteration " << i << "\n";
			break;
		}

		int j = rand() % r.size();
		k.push_back(r[j]);
		r.erase(r.begin() + j);
	}

	return k;
}

void dump(const string& publicp, const string& privatep, const string& outp){

	ifstream img(privatep, std::ios::binary);

	if(!img.is_open()){
		cout << "bunk: source img crash";
		img.close();
		return;
	}

	ifstream exec(publicp, std::ios::binary);

	if(!exec.is_open()){
		cout << "bunk: source exe crash";
		img.close();
		exec.close();
		return;
	}

	ofstream out(outp, std::ios::binary);

	if(!out.is_open()){
		cout << "bunk: target img crash";
		img.close();
		exec.close();
		out.close();
		return;
	}

	cout << "bunk: binaries open\n";

	// first
	out << img.rdbuf();

	// sig
	unsigned int sfi = INT_FAST32_MIN;
	img.clear();
	img.seekg(1024);

	img.read(reinterpret_cast<char *>(&sfi), sizeof(sfi));

	if(sfi == INT_FAST32_MIN){
		cout << "bunk: failed getting seed from image\n";
		img.close();
		exec.close();
		out.close();
		return;
	}

	cout << "bunk: image seed given as " << sfi << "\n";
	
	vector<byte> sig = signature(sfi);

	for(char c : sig){
		out.put(c);
	}

	cout << "bunk: put signature\n";

	// second

	vector<byte> vige = vigenerekey(sfi);

	std::size_t nbytes = 2000000;
	vector<char> buf(nbytes);
	while(exec){
		exec.read(buf.data(), nbytes);
		const auto nread = exec.gcount();
		if(!nread)
			break;

		vector<byte> bytes(buf.begin(), buf.begin() + nread);
		for(int i=0; i<bytes.size(); i++){
			byte c = bytes[i];
			out.put(vige[c]);

		}
		
	}


	// out << exec.rdbuf();

	cout << "bunk: encrypted bytes dumped into " << outp << "\n";

	img.close();
	exec.close();
	out.close();

	cout << "bunk: binaries closed\nbunk: out file at " << outp << std::endl;
}

void extract(const string& bunkp, const string& outp) {

    std::ifstream bnk(bunkp, std::ios::binary);
    if(!bnk.is_open()){
        cout << "bunk: source img crash" << std::endl;
		bnk.close();
        return;
    }

    std::ofstream out(outp, std::ios::binary);
    if(!out.is_open()){
        cout << "bunk: target exe crash" << std::endl;
		bnk.close();
		out.close();
        return;
    }

    const std::size_t nbytes = 2000000; // 2mb in buff
    vector<char> buf(nbytes);

	unsigned int sfi = INT_FAST32_MIN;

	bnk.clear();
	bnk.seekg(1024);
	bnk.read(reinterpret_cast<char*>(&sfi), sizeof(sfi));

	if(sfi == INT_FAST32_MIN){
		cout << "bunk: failed getting seed from image\n";
		bnk.close();
		out.close();
		return;
	}

	cout << "bunk: image seed given as " << sfi << "\n";

	vector<byte> bunksig = signature(sfi);

	bnk.clear();
	bnk.seekg(0, std::ios::beg);

    if(bnk.read(buf.data(), buf.size()) || bnk.gcount() > 0){
        const auto nread = bnk.gcount();
        cout << "bunk: read " << nread << " bytes from input " << bunkp << "\n";

        vector<byte> bytes_find(buf.begin(), buf.begin() + nread);

        auto it = std::search(bytes_find.begin(), bytes_find.end(), bunksig.begin(), bunksig.end());

        if(it != bytes_find.end()){
            int idx = std::distance(bytes_find.begin(), it) + bunksig.size();
            cout << "bunk: signature found at byte " << idx - bunksig.size() << ", dumping bytes to " << outp << "\n";

			// writing to out
			bnk.clear();
			bnk.seekg(idx);

			if(!bnk){
				cout << "bunk: failure navigating binary " << bunkp << "\n";
				bnk.close();
				out.close();
				return;
			}

			vector<byte> vige = vigenerekey(sfi);
			buf.resize(nbytes);

			while(bnk){
				bnk.read(buf.data(), nbytes);
				const auto nread = bnk.gcount();
				if(!nread)
					break;

				vector<byte> bytes_out(buf.begin(), buf.begin() + nread);
				for(int i=0; i<bytes_out.size(); i++){
					byte c = bytes_out[i];
					auto iterator_loc = std::find(vige.begin(), vige.end(), c);

					if(iterator_loc==vige.end()){
						cout << "bunk: decryption failure on encrypted byte " << c << "\n";
						bnk.close();
						out.close();
						return;
					}

					int o = iterator_loc - vige.begin();
					// cout << c << " to " << static_cast<char>(o) << "\n";
					out.put(o);
				}
			}

            // out.write(reinterpret_cast<char*>(&bytes[idx]), nread - idx);

            // while (bnk.read(buf.data(), buf.size())) { // i hate buffers!
            //     out.write(buf.data(), buf.size());
			// 	cout << "bunk: read " << buf.size() << " bytes from input " << bunkp << "\n";
            // }

            // out.write(buf.data(), bnk.gcount()); // see above

            cout << "bunk: decrypted bytes dumped into " << outp << "\n";
        }else{
            cout << "bunk: failed to find signature" << std::endl;
			bnk.close();
			out.close();
			return;
        }

        bnk.close();
        out.close();

        cout << "bunk: out file at " << outp << std::endl;
    } else {
        cout << "bunk: failed to read input " << bunkp << std::endl;
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
	
	else if(splargs[0]=="extract")
		extract(splargs[1], splargs[2]);
	
}