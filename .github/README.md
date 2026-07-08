# BarleyMatch

PVZ（植物大战僵尸）自动化测试框架，支持通过 Lua 脚本配置测试环境，实现大规模植物强度测试。

同时支持手动模式，实现手动开局和观战。

## 功能特性

- **Lua 脚本配置**：通过 `MatchConfig.lua` 灵活定义测试规则
- **自动化测试**：支持多实例并行测试，自动循环执行
- **状态机驱动**：清晰的对局生命周期管理
- **数据输出**：可通过 lua 脚本自动输出测试结果

## 构建

项目使用 [vcpkg](https://github.com/microsoft/vcpkg) 管理依赖：

```bash
# 安装依赖
vcpkg install

# 使用 Visual Studio 打开 Match.slnx 构建
```

## 使用方法

### 1. 配置测试环境

将 `example/MatchConfig.lua` 复制到工作目录，按需修改：

```lua
-- 基础配置
function isPoolEnabled()
    return false  -- 是否启用泳池（5行/6行）
end

function isAutoMode()
    return true  -- 是否自动模式
end

-- 定义植物池、僵尸池等
```

### 2. 配置加速参数

编辑 `example/file.json`：

```json
{
    "acceleration_factor": 250,  // 加速倍率
    "draw_board": false,         // 是否绘制场景
    "venv": ["PVZ安装路径"]
}
```

### 3. 运行测试

1. 启动 PVZ 游戏
2. 运行 `AutoTest-Setup` 注入测试模块
3. 测试自动开始，结果由 Lua 脚本输出

## Lua API 参考

### 配置函数

初始化时调用，必须实现：

| 函数 | 返回值 | 说明 |
|------|--------|------|
| `isPoolEnabled()` | `bool` | 是否启用泳池（true=6行，false=5行） |
| `isAutoMode()` | `bool` | 是否为自动对战模式 |

### 回调函数

在对应生命周期由 C++ 调用：

| 函数 | 参数 | 调用时机 |
|------|------|----------|
| `OnMatchInit()` | 无 | 对局初始化，用于布置植物阵型 |
| `OnPreMatch()` | 无 | 赛前准备阶段每帧调用 |
| `OnMatchUpdate()` | 无 | 比赛进行时每帧调用 |
| `OnTeamEliminated(row)` | `row`: 被淘汰的行号 | 小推车触发时调用 |
| `OnTerminate()` | 无 | 对局结束时调用 |

### 导出函数

可在 Lua 中直接调用：

| 函数 | 参数 | 说明 |
|------|------|------|
| `CreatePlant(type, row, column)` | 植物类型、行、列 | 在指定位置创建植物 |
| `CreateZombie(type, row, column, x)` | 僵尸类型、行、列、X坐标 | 在指定位置创建僵尸 |
| `ClearPlants()` | 无 | 清除场上所有植物 |
| `Terminate()` | 无 | 终止当前对局，触发 `OnTerminate()` |
| `HasZombie()` | 无 | `bool` | 判断场上是否有僵尸 |

### Match 属性

通过 `Match.xxx` 读写游戏状态：

| 属性 | 读写 | 说明 |
|------|------|------|
| `Match.StateCountdown` | 读写 | 阶段转换倒计时，归零时进入下一阶段 |
| `Match.PrimaryCounter` | 读写 | 游戏时间计数器（PlayingTime） |
| `Match.SecondaryCounter` | 读写 | 僵尸生成倒计时 |
| `Match.TertiaryCounter` | 读写 | 通用计时器 |
| `Match.Round` | 读写 | 已完成对局数 |
| `Match.RowsPerRound` | 只读 | 每回合行数（5或6） |
