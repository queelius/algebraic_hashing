#include <gtest/gtest.h>
#include <iostream>
#include <packed_container/bit_view.h>
#include <vector>

TEST(BitView, BitIterator)
{
    const size_t N = 50;
    char data[N];
    alex::bits::bit_view bv(&data, 0, N * 8);

    int cnt = 0;
    for (size_t i = 0; i < N * 8; ++i)
    {
        auto val = ((i % 3) == 0 || (i % 7) == 0);
        if (val)
            ++cnt;
        bv[i] = val;
    }

    size_t i = 0;
    for (auto x : bv)
    {
        ASSERT_EQ((bool)x, (i % 3 == 0) || (i % 7 == 0));
        ++i;
    }

    ASSERT_EQ(std::count(bv.begin(), bv.end(), true), cnt);
}

TEST(BitView, BitViewLarge)
{
    const size_t N = 20000;
    char data[N];
    alex::bits::bit_view bv(&data, 0, N * 8);

    for (size_t i = 0; i < N * 8; ++i)
        bv[i] = (i % 7) == 0 || (i % 19) == 0;

    EXPECT_EQ(bv.size(), N * 8);
    for (size_t i = 0; i < N * 8; ++i)
        bv[i] == (i % 7) == 0 || (i % 19) == 0;
}

TEST(BitView, BitViewSet)
{
    char data[2];
    alex::bits::bit_view bv(&data, 0, 16);

    bv.set();
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(bv[i].is_set(), true);
}

TEST(BitView, BitViewClear)
{
    char data[2];
    alex::bits::bit_view bv(&data, 0, 16);

    bv.clear();
    for (int i = 0; i < 16; ++i)
        EXPECT_EQ(bv[i].is_set(), false);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}