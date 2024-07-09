#ifndef OFFSET_LOG_H
#define OFFSET_LOG_H

#include "Common.h"
#include "GV.h"

namespace UnPdeC {

    /// <summary>
    /// 偏移记录类
    /// </summary>
    class OffsetLog {
    public:
        /// <summary>
        /// 添加偏移记录
        /// </summary>
        /// <param name="BlockOffset">数据块描述信息来自哪里的偏移值</param>
        /// <param name="OriginalOffset">描述块中的原始偏移值</param>
        /// <param name="PdeOffset">描述块原始偏移值转换出来的PDE文件中实际偏移值</param>
        /// <param name="Size">此数据的大小</param>
        /// <param name="Name">名称</param>
        /// <param name="Type">类型</param>
        /// <param name="Path">路径</param>
        static void AddEntry(uint32_t BlockOffset, uint32_t OriginalOffset, uint32_t PdeOffset, uint32_t Size,
            std::string_view Name, uint8_t Type, std::string_view Path);

        /// <summary>
        /// 获取或创建路径
        /// </summary>
        /// <param name="Path">路径</param>
        /// <returns>*LogData</returns>
        static nlohmann::json& GetOrCreatePath(std::string_view Path);

        /// <summary>
        /// 保存JSON文件到硬盘
        /// </summary>
        static void SaveToFile();

        /// <summary>
        /// 写入全局变量
        /// </summary>
        static void WriteToGV();

    private:
        /// <summary>
        /// 偏移日志数据
        /// </summary>
        static nlohmann::json LogData;

        /// <summary>
        /// 初始化偏移值日志
        /// </summary>
        static void InitializeLogDataIfNeeded();

        /// <summary>
        /// 10进转16进制
        /// </summary>
        /// <param name="value">10进制</param>
        /// <returns>16进制</returns>
        static std::string ToHex(uint32_t value);
    };

} // namespace UnPdeC
#endif // OFFSET_LOG_H