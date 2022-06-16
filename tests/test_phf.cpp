#include <perfect_hashing/phf_builder.hpp>
#include <hashing/fnv_hash.hpp>
#include <perfect_hashing/phf.hpp>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <chrono>

void test_phf();

int main()
{
    test_phf();
}

void test_phf()
{
    std::vector<std::string> s(200);
    s[0] = "23klsfjdd;14asdfsdfatasdfasfg";
    s[1] = "12klsfjdjfaasdfdbryujjaaff2333d;1eeefff4tg";
    s[2] = "01kfjdlajsaafd;14tgeee333";
    s[3] = "95klsfjdfaflajfd;1ddd4tg";
    s[4] = "8aaaaaa2232366kdddladfasfjdlajfd;14tg";
    s[5] = "7eeee66kladsdf554fcccasfjdlajfd;14tg";
    s[6] = "6dddd66kbbbbladfasfjdlajfd;14tg";
    s[7] = "5ccc66kladfasfjdlajfd;14adfadfftg";
    s[8] = "4bbbb66kladfasfjdlajfd;14taag";
    s[9] = "3aaa66kladfasfjdlajfd;14afffdafaa45455566tg";
    s[10] = "2a16kladfasfjdlajfd;14asdfasdasfatg";
    s[11] = "1b66kladfasfjdlajfdasdfasasdf45443;14tg";
    s[12] = "0c66kladfasdfasdf454asfjdlajfd;14tg";
    s[13] = "9d66kladfasfjdlajfd;1242423423421355634asdfasdf4tg";
    s[14] = "8e3klsfadfadfdjdlajfd;143453234tg";
    s[15] = "7f2klsfjdlajfaaaff2333d;1353454tg";
    s[16] = "6g1klsfjdlajsaafd;14tg3332342323456";
    s[17] = "5h5klsfjdfaflajfd;14tg";
    s[18] = "4i66kladfa6gadfsfjdlajfd;14t234243a";
    s[19] = "3j66kladfasfjd11111111111334234lajfd;14tb";
    s[20] = "2k66kladfasfjdl2342333333ajfd;14tc";
    s[21] = "1l66kladfasfj2342dlajfd;14td";
    s[22] = "0m66kladfasfjdlajfd;23423423414te";
    s[23] = "9n66kladfasadfafaukr566ygfjdla23423jfd;14tf";
    s[24] = "8o66kladfasfjdlajfd;14tg";
    s[25] = "7p66kladfasfjdlaj554fd;14th";
    s[26] = "6q66kladfassfgsgjdlajfd;14ti";
    s[27] = "5r66kladasdfasdfasdefasfjdlajfd;14tj";
    s[28] = "4s3klsfjdlajfd;1sdfs4tg";
    s[29] = "3t2klsfjdlajfaaaff2333asdfdd;14tgadfasfaf";
    s[30] = "2u1klsfjdlajsaafd;14tg333";
    s[31] = "1v5klsfjdfaflajfdsdf;14tg";
    s[32] = "0w166kladfasfjdlajfd;14tg";
    s[33] = "9x1626kladfasfjdlajfd;14tg";
    s[34] = "8y66kl3adsddfg7777888fasfjdlajfd;14tg";
    s[35] = "7z66kla4dasdfasfadsfasfjdlajfd;14tg";
    s[36] = "6zz66kla5dfasfjasdfasfasdasfdlajfd;14tg";
    s[37] = "5zzz66kla6dfasfjdlajfd;14tg";
    s[38] = "4zzzz66kla7dfa777sfjdlajfd;714777tg";
    s[39] = "3zzzzz66kla8dfasfjdlajfd;1477777tg77";
    s[40] = "2zzzzzz66kla9dfasfjdlajfd;14tg";
    s[41] = "1zzzzzzz66kla000dfasfjdlajfd;14tg";
    s[42] = "23klsfjdlajfd;11000012233314tg";
    s[43] = "12klsfjdlajfaaaff233111113d33311;33sd5aab331123214tg";
    s[44] = "01klsfjdlajsaafd;14tg322111111223332asfdasfs33";
    s[45] = "95klsfjdfaflajfd;hghs14tg333ada1322342322";
    s[46] = "8aaaaaa2232366kklllklad2222fasfjdlajf323423aaavbb2423243saa333d;14tg";
    s[47] = "7eeee66kladfasf111jdlajfd;14tadsffd11111g";
    s[48] = "6dddd66kladf111asfjdlajfd;14adfafatg";
    s[49] = "5ccc66kladf1asfj676jdlajfd;14tg";
    s[50] = "2a16kladfa-sfjasdfsdsds2333444dlajfyd;14tg";
    s[51] = "1b66kladfa1sfjy6e56yyebdlajfd;14tg";
    s[52] = "0c66kladf0asfjdlajfd;g56ee14tg";
    s[53] = "9d66klad9fasfjdlajfd;14tg";
    s[54] = "8e3klsf8jdlajfd;14tg";
    s[55] = "7f2kls7fjdlajfaaaff2333d;14tg";
    s[56] = "6g1kl6sfjdlajsaafd;14tg333";
    s[57] = "5h5k5lsfjdfafl56363ajfd;14tg";
    s[58] = "4i646kladfasfjdlaj563asdfasffd;14tg";
    s[59] = "3j366kladfasfjdlajfadfafasdfffda111asdsfdsgd;14tg";
    s[60] = "22k66kladfasfjd124435lajfd;14354566tg";
    s[61] = "11l66kladfasasdfasdasdffjdlajf55d;14tg";
    s[62] = "0m66aladfasfjdla555aa344jfdadfadf;155666774t4";
    s[63] = "9n66bladfasasdfasdfadfsadfadsfhjjufjdlajfd;144aafg";
    s[64] = "8o66cladfasfjdlaasdasdfaffhhhh44jfuioiopd;144";
    s[65] = "7p66dladfaiasdfdsaasdasfdafsdfaasdfasdffasd45555alioluisfjdlajfd;414tg";
    s[66] = "6q66eladfasfjdlajioiasfoyksfd4;14tg";
    s[67] = "5r66fladfasfjdlasdadffsdfasdasd344445ajf4dsgs;14tafaf";
    s[68] = "4s3kgsfjdlajfasasdffd;1asdfsdfd4544444444tg222445";
    s[69] = "3t2kghsfjdlaadjfasdf45545555aaa4ff2333d;14t11233gf";
    s[70] = "2uasdf1klsfjdlajsaa4fd;14tg333";
    s[71] = "1v5klsafjdfagfla4jfd;14tg";
    s[72] = "0w66klaadfasfjs4dlajfd;14sgfsdgsfd";
    s[73] = "9x66kladbfasf4jdlajfbvcvbd;14";
    s[74] = "8y66kladfbas4fjd6lertrajfd;14";
    s[75] = "7z66kladfad4sfjd4lajfd;14tgrrr";
    s[76] = "6zz66klad4feasf355jdlajfrttrrtd;14";
    s[77] = "5zzz66kl4adfeta5sfjdlajfd;14tgrrt";
    s[78] = "4zaazzz664klaaf55sgdfadfasfjdlajfd;14";
    s[79] = "3zzzaazz466kladf5afsfasfdfjdlajfd;14tg";
    s[80] = "2zzzz4aabbzz66kladfafsfjdlaafddfadfjfd;14";
    s[81] = "1zzz4zzzz66klad55fadafasfjdlajfd;14tasdfadfg";
    s[82] = "1zz4zzzzz66kla5dfasfjdfdflajfd;14";
    s[83] = "1z4zzzz111zz655555556kladfasfjddadfflajfd;14";
    s[84] = "14zz443344zzzzz6116kladfasfjdlajfd;adsf555514";
    s[85] = "41zzahgh8899555zzzzz66kasdffdads1111ladfasfjdlajfd;4414";
    s[86] = "1zzzsdfgsdgsgzzzz66kladfa111sfjdlajfd4;14afdsdsd";
    s[87] = "1zzzzz66kasdfsdfasdfasdfladfasfj1111dlajf4d;143r5434";
    s[88] = "1zzz66kladfasfjdlajfd;14tg";
    s[89] = "1z66kladasdfadfadsdfasfjdla4114jfd;14345434";
    s[90] = "12334444zzz66kladfasfjdlajfd1;14tfaffafg";
    s[91] = "1zzz1zzz66kladfasfjdl4ajfd1;14tfafdsfag1111223456";
    s[92] = "1zz3zzabcz66kladfasfjd4lajfd1111;14tg";
    s[93] = "1z5zzzaaffggzz6644kasdfasdfsdafladfas4fjdlaj11fd;afdasdf14";
    s[94] = "1zz77zzzadfsfasz664k4ladfa4sfjdlajf111d;14tg";
    s[95] = "1zzzz8zz6446k4lad4fasfjdlajf1d;14tg";
    s[96] = "1zzzz9adfasfhjiiiozzz66klaasfdsdfadsfafd444fasfjdlajfd;114";
    s[97] = "1zz99llsszzz934334z4663kladfasfjdlajfd;14a1";
    s[98] = "adfaggaa331zs666hjhkzzzadfasaasdfzzz66kladfasfjdlaaajfd;14";
    s[99] = "ggg1zzzzzret5454zz66adsfasfadasdfdkladfasfjdlajfd;14taag";
    s[100] = "123klsfjdd;14tasdfasfg";
    s[101] = "212klsfjdjfaaaff2333d;1eeefff4tg";
    s[102] = "301kfjdlajsaafd;14tgeee333";
    s[103] = "495klsfjdfaflajfd;1ddd4tg";
    s[104] = "58aaaaaa2232366kdddladfasfjdlajfd;14tg";
    s[105] = "67eeee66kladfcccasfjdlajfd;14tg";
    s[106] = "76dddd66kbbbbladfasfjdlajfd;14tg";
    s[107] = "85ccc66kladfasfjdlajfd;14adfadfftg";
    s[108] = "94bbbb66kladfasfjdlajfd;14taag";
    s[109] = "03aaa66kladfasfjdlajfd;14afffdafaa45455566tg";
    s[110] = "12a16kladfasfjdlajfd;14asdfasdasfatg";
    s[111] = "21b66kladfasfjdlajfdasdfasasdf45443;14tg";
    s[112] = "30c66kladfasdfasdf454asfjdlajfd;14tg";
    s[113] = "49d66kladfasfjdlajfd;1242423423421355634asdfasdf4tg";
    s[114] = "58e3klsfjdlajfd;143453234tg";
    s[115] = "67f2klsfjdlajfaaaff2333d;1353454tg";
    s[116] = "76g1klsfjdlajsaafd;14tg3332342323456";
    s[117] = "85h5klsfjdfaflajfd;14tg";
    s[118] = "94i66kladfasfjdlajfd;14t234243a";
    s[119] = "03j66kladfasfjd11111111111334234lajfd;14tb";
    s[120] = "12k66kladfasfjdl2342333333ajfd;14tc";
    s[121] = "21l66kladfasfj2342dlajfd;14td";
    s[122] = "30m66kladfasfjdlajfd;23423423414te";
    s[123] = "49n66kladfasfjdla23423jfd;14tf";
    s[124] = "58o66kladfasfjdlajfd;14tg";
    s[125] = "67p66kladfasfjdlaj554fd;14th";
    s[126] = "76q66kladfasfjdlajfd;14ti";
    s[127] = "85r66kladasdfasdfasdefasfjdlajfd;14tj";
    s[128] = "94s3klsfjdlajfd;14tg";
    s[129] = "03t2klsfjdlajfaaaff2333d;14tg";
    s[130] = "12u1klsfjdlajsaafd;14tg333";
    s[131] = "21v5klsfjdfaflajfd;14tg";
    s[132] = "30w166kladfasfjdlajfd;14tg";
    s[133] = "49x1626kladfasfjdlajfd;14tg";
    s[134] = "58y66kl3adfasfjdlajfd;14tg";
    s[135] = "67z66kla4dfasfjdlajfd;14tg";
    s[136] = "76zz66kla5dfasfjdlajfd;14tg";
    s[137] = "85zzz66kla6dfasfjdlajfd;14tg";
    s[138] = "94zzzz66kla7dfasfjdlajfd;14tg";
    s[139] = "03zzzzz66kla8dfasfjdlajfd;14tg";
    s[140] = "12zzzzzz66kla9dfasfjdlajfd;14tg";
    s[141] = "21zzzzzzz66kla000dfasfjdlajfd;14tg";
    s[142] = "3323klsfjdlajfd;11000012233314tg";
    s[143] = "344412klsfjdlajfaaaff233111113d33311;33331123214tg";
    s[144] = "5555501klsfjdlajsaafd;14tg322111111223332asfdasfs33";
    s[145] = "6666695klsfjdfaflajfd;hghs14tg3331322342322";
    s[146] = "7777778aaaaaa2232366kklllklad2222fasfjdlajf3234232423243saa333d;14tg";
    s[147] = "888887eeee66kladfasf111jdlajfd;14tadsffd11111g";
    s[148] = "8888999006dddd66kladf111asfjdlajfd;14adfafatg";
    s[149] = "15ccc66asdfadfsdfakladf1asfj676jdlajfd;14tg";
    s[150] = "22a16kladfa-sfjasdfsdsds2333444dlajfyd;14tg";
    s[151] = "31b66kladfa1sfjy6e56yyebdlajfd;14tg";
    s[152] = "40c66kladf0asfjdlajfd;g56ee14tg";
    s[153] = "59d66klad9fasfjdlajfd;14tg";
    s[154] = "68e3klsf8jdlajfd;14tg";
    s[155] = "77f2kls7fjdlajfaaaff2333d;14tg";
    s[156] = "86g1kl6sfjdlajsaafd;14tg333";
    s[157] = "95h5k5lsfjdfafl56363ajfd;14tg";
    s[158] = "04i646kladfasfjdlaj563asdfasffd;14tg";
    s[159] = "00000000000000000011233j366kladfasfjdlajfadfafasdfffda111asdsfdsgd;14tg";
    s[160] = "a22k66kladfasfjd124435lajfd;14354566tg";
    s[161] = "b11l66kladfasasdfasdasdffjdlajf55d;14tg";
    s[162] = "c0m66aladfasfjdla555aa344jfdadfadf;155666774t4";
    s[163] = "d9n66bladfasasdfasdfadfsadfadsfhjjufjdlajfd;144aafg";
    s[164] = "e8o66cladfasfjdlaasdasdfaffhhhh44jfuioiopd;144";
    s[165] = "f7p66dladfaiasdfdsaasdasfdafsdfaasdfasdffasd45555alioluisfjdlajfd;414tg";
    s[166] = "g6q66eladfasfjdlajioiasfoyksfd4;14tg";
    s[167] = "i5r66fladfasfjdlasdadffsdfasdasd344445ajf4dsgs;14tafaf";
    s[168] = "j4s3kgsfjdlajfasasdffd;1asdfsdfd4544444444tg222445";
    s[169] = "k3t2kghsfjdlaadjfasdf45545555aaa4ff2333d;14t11233gf";
    s[170] = "l2uasdf1klsfjdlajsaa4fd;14tg333";
    s[171] = "m1v5klsafjdfagfla4jfd;14tg";
    s[172] = "n0w66klaadfasfjs4dlajfd;14sgfsdgsfd";
    s[173] = "o9x66kladbfasf4jdlajfbvcvbd;14";
    s[174] = "p8y66kladfbas4fjd6lertrajfd;14";
    s[175] = "q7z66kladfad4sfjd4lajfd;14tgrrr";
    s[176] = "r6rzz66klad4feasf355jdlajfrttrrtd;14";
    s[177] = "s5zzz66kl4adfeta5sfjdlajfd;14tgrrt";
    s[178] = "w4zaazzz664klaaf55sgdfadfasfjdlajfd;14";
    s[179] = "u3zzzaazz466kladf5afsfasfdfjdlajfd;14tg";
    s[180] = "v2zzzz4aabbzz66kladfafsfjdlaafddfadfjfd;14";
    s[181] = "w1zzz4zzzz66klad55fadafasfjdlajfd;14tasdfadfg";
    s[182] = "x1zz4zzzzz66kla5dfasfjdfdflajfd;14";
    s[183] = "y1z4zzzz111zz655555556kladfasfjddadfflajfd;14";
    s[184] = "zzzz14zz443344zzzzz6116kladfasfjdlajfd;adsf555514";
    s[185] = "zzzzzzzzz2334435641zzahgh8899555zzzzz66kasdffdads1111ladfasfjdlajfd;4414";
    s[186] = "11223asdfa661zzzsdfgsdgsgzzzz66kladfa111sfjdlajfd4;14afdsdsd";
    s[187] = "343661zzzzz66kasdfsdfasdfasdfladfasfj1111dlajf4d;143r5434";
    s[188] = "65651zzz66kladfasfjdlajfd;14tg";
    s[189] = "12324334z66kladasdfadfadsdfasfjdla4114jfd;14345434";
    s[190] = "1112546udgh12334444zzz66kladfasfjdlajfd1;14tfaffafg";
    s[191] = "11zzz1zzz66asdfasdfkladfasfjdl4ajfd1;14tfafdsfag1111223456";
    s[192] = "21zz3zzabcz66kladfasfjd4lajfd1111;14tg";
    s[193] = "31z5zzzaaffggzz6644kasdfasdfsdafladfas4fjdlaj11fd;afdasdf14";
    s[194] = "41zz77zzzadasdfasdffsfasz664k4ladfa4sfjdlajf111d;14tg";
    s[195] = "51zzzz8zz6aasdf446k4lad4fasfjdlajf1d;14tg";
    s[196] = "61zzzz9adfasfhjiiiozzz66klaasfdsdfadsfafd444fasfjdlajfd;114";
    s[197] = "71zz99llsszzz934334z4663kladfasfjdlajfd;14a1";
    s[198] = "8adfaggaa331zs666hjhkzzzadfasaasdfzzz66kladfasfjdlaaajfd;14";
    s[199] = "9ggg1zzzzzret5454zz66adsfasfadasdfdkladfasfjdlajfd;14taag";
    
    auto start = std::chrono::system_clock::now();

    auto ph = perfect_hashing::phf_builder<hashing::fnv_hash>().
        load_factor(.2).
        timeout(std::chrono::seconds(5)).
        index(0,20000000)(s);

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << (double)elapsed.count() / 1000 << '\n';
    std::cout << "error rate = " << ph.error_rate() * 100 << "%\n";

    std::unordered_set<size_t> hashes;
    for (auto x : s)
    {
        auto h = ph(x);
        if (hashes.count(h) != 0)
            std::cout << "collision: ph(" << x << ") = " << h << "\n";
        hashes.insert(h);
    }
}
