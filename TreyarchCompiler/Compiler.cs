using TreyarchCompiler.Enums;
using TreyarchCompiler.Games;
using TreyarchCompiler.Interface;
using TreyarchCompiler.Utilities;

namespace TreyarchCompiler
{
    public class Compiler
    {
        public static CompiledCode Compile(Platforms platform, Enums.Games game, Modes mode, bool uset8masking, string code, string path = "")
        {
            ICompiler compiler;
            switch (game)
            {
                case Enums.Games.T7:
                    compiler =  new GSCCompiler(mode, code, path, platform, game, false);
                    break;
                case Enums.Games.T8:
                case Enums.Games.T937:
                case Enums.Games.T9:
                    compiler = new T89Compiler(game, platform, code);
                    break;
                default:
                    return null;
            }
            return compiler.Compile();
        }
    }
}