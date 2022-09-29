#include "cryptolab/GaloisField2Vector.h"
#include "cryptolab/AES128.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace cryptolab;

int main()
{
    gf28::setMx(0x1b);
    AES128 f;
    cout << "设置密钥(16个字符，超出部分会被截断): ";
    string tmpStr;
    getline(cin, tmpStr);
    f.setInitKey(tmpStr.c_str());
    cout << "工作模式(ecb|cbc)：";
    getline(cin, tmpStr);
    AES128::WorkingMode wm;
    if (tmpStr == "ecb")
        wm = AES128::WorkingMode::ECB;
    else if (tmpStr == "cbc")
    {
        wm = AES128::WorkingMode::CBC;
        cout << "初始向量(16个字符，超出部分会被截断)：";
        getline(cin, tmpStr);
        f.setInitVec(tmpStr.c_str());
    }
    else
    {
        wm = AES128::WorkingMode::ECB;
        cout << "默认选择ECB模式." << endl;
    }
    cout << "填充模式(nopad|zeros|iso10126|pkcs5)：";
    getline(cin, tmpStr);
    AES128::PaddingType pt;
    if (tmpStr == "nopad")
        pt = AES128::PaddingType::NOPADDING;
    else if (tmpStr == "zeros")
        pt = AES128::PaddingType::ZEROS;
    else if (tmpStr == "iso10126")
        pt = AES128::PaddingType::ISO10126;
    else if (tmpStr == "pkcs5")
        pt = AES128::PaddingType::PKCS5;
    else
    {
        cout << "默认选择ISO10126模式." << endl;
        pt = AES128::PaddingType::ISO10126;
    }
    cout << "输入明文: ";
    getline(cin, tmpStr);
    auto c = f.encrypt(tmpStr, pt, wm);
    for (int i = 0; i < 44; ++i)
    {
        cout << "W[" << dec << i << "] = " << setw(2) << setfill('0') << hex << f.getW(i) << "  ";
        if (i % 4 == 3)
            cout << endl;
    }
    cout << "加密后的密文(16进制)为: " << hex << c << endl;
    cout << "解密后的明文为：" << f.decrypt(c, pt, wm).toString() << endl;
    cout << "是否写出到文件(y|n)? ";
    getline(cin, tmpStr);
    if (tmpStr == "y")
    {
        ofstream ofile;
        string fn;
        cout << "写出格式(hex|bin): ";
        getline(cin, tmpStr);
        cout << "文件名: ";
        getline(cin, fn);
        if (tmpStr == "bin")
        {
            cout << "已选取bin模式." << endl;
            ofile.open(fn, ios::out|ios::binary);
            ofile.write((const char *)c.data(), c.size());
            ofile.close();
        }
        else
        {
            cout << "已选取hex模式." << endl;
            ofile.open(fn, ios::out);
            ofile << hex << c;
            ofile.close();
        }
    }
    cout << "是否读取文件并使用相同设置解密(y|n)? ";
    getline(cin, tmpStr);
    if (tmpStr == "y")
    {
        ifstream ifile;
        string fn;
        cout << "文件格式(hex|bin): ";
        getline(cin, tmpStr);
        cout << "文件名: ";
        getline(cin, fn);
        if (tmpStr == "bin")
        {
            cout << "已选取bin模式." << endl;
            ifile.open(fn, ios::in|ios::binary);
            ifile.seekg(0, ios::end);
            long fLength = ifile.tellg();
            ifile.seekg(0, ios::beg);
            c.resize(fLength);
            ifile.read((char *)c.data(), fLength);
            ifile.close();
        }
        else
        {
            cout << "已选取hex模式." << endl;
            ifile.open(fn, ios::in);
            ifile.seekg(0, ios::end);
            long fLength = ifile.tellg();
            ifile.seekg(0, ios::beg);
            char *buf = new char[fLength + 1];
            ifile.read(buf, fLength);
            buf[fLength] = '\0';
            ifile.close();
            c = ByteBuffer::fromHexStr(buf);
            delete[] buf;
        }
        cout << "读取到的密文为: " << c << endl;
        c = f.decrypt(c, pt, wm);
        cout << "解密后的明文为：" << c.toString() << endl;
    }
    return 0;
}
