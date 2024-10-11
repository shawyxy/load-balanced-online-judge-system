// 如果编译选项没有定义宏，那么引入代码接口
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

// 随机数生成器类
class RandomGenerator
{
public:
    // 生成随机数组
    static vector<int> generateRandomArray(int length, int minVal, int maxVal)
    {
        vector<int> nums;
        for (int i = 0; i < length; ++i)
        {
            int randomNum = minVal + rand() % (maxVal - minVal + 1);
            nums.push_back(randomNum);
        }
        return nums;
    }

    // 生成随机目标值
    static int generateRandomTarget(int minVal, int maxVal)
    {
        return minVal + rand() % (maxVal - minVal + 1);
    }
};

// 标准答案实现
class CorrectSolution
{
public:
    vector<int> twoSum(vector<int> &nums, int target)
    {
        unordered_map<int, int> map;
        for (int j = 0; j < nums.size(); j++)
        {
            auto it = map.find(target - nums[j]);
            if (it != map.end())
            {
                return {it->second, j};
            }
            map[nums[j]] = j;
        }
        return {};
    }
};

// 对数器
void checker(int testCases, int arrayLength, int minVal, int maxVal)
{
    Solution sol;
    CorrectSolution correct_sol;

    srand(time(0)); // 使用当前时间作为随机种子

    for (int i = 0; i < testCases; ++i)
    {
        vector<int> nums = RandomGenerator::generateRandomArray(arrayLength, minVal, maxVal);
        int target = RandomGenerator::generateRandomTarget(minVal, maxVal);

        // 使用两种方法计算
        vector<int> result1 = sol.twoSum(nums, target);
        vector<int> result2 = correct_sol.twoSum(nums, target);

        // 结果对比
        if (result1 != result2)
        {
            cout << "Error: Mismatch found!" << endl;
            cout << "Array: ";
            for (int num : nums)
            {
                cout << num << " ";
            }
            cout << endl;
            cout << "Target: " << target << endl;
            cout << "Standard solution result: [" << result1[0] << ", " << result1[1] << "]" << endl;
            cout << "Brute force result: [" << result2[0] << ", " << result2[1] << "]" << endl;
            return;
        }
    }
    cout << "All test cases passed!" << endl;
}

int main()
{
    // 对数器参数：测试用例数，数组长度，最小值，最大值
    checker(1000, 100, -1000, 1000);
    return 0;
}
