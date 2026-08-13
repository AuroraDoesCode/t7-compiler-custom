using System;
using System.Collections.Generic;
using System.IO;

namespace T89CompilerLib.ScriptComponents
{
    public sealed class T89RequiresImplementsSection : T89ScriptSection
    {
        /// <summary>
        /// Internal list of implements
        /// </summary>
        public HashSet<ulong> Implements = new HashSet<ulong>();

        public T89ScriptObject Script { get; private set; }
        private T89RequiresImplementsSection(T89ScriptObject script) { Script = script; } //We dont want initializations of this class without our deserialization procedures

        public static T89RequiresImplementsSection New(T89ScriptObject script)
        {
            T89RequiresImplementsSection section = new T89RequiresImplementsSection(script);
            section.Implements = new HashSet<ulong>();
            return section;
        }

        /// <summary>
        /// Add an implement to this script
        /// </summary>
        /// <param name="Implement"></param>
        public void Add(ulong Implement)
        {
            if (Implements.Count > 256)
            {
                // stored using a byte
                throw new Exception("Can't add more than 256 implements to a script");
            }
            Implements.Add(Implement);
        }

        /// <summary>
        /// Remove an implement from this script
        /// </summary>
        /// <param name="Implement"></param>
        public void Remove(ulong Implement)
        {
            Implements.Remove(Implement);
        }

        /// <summary>
        /// Number of includes in this section
        /// </summary>
        /// <returns></returns>
        public override ushort Count()
        {
            return (ushort)Implements.Count;
        }

        /// <summary>
        /// Returns the section size. For includes, this consists of the string and the reference for each include.
        /// </summary>
        /// <returns></returns>
        public override uint Size()
        {
            uint count = (uint)Implements.Count * 8;

            uint Base = GetBaseAddress();

            count = (Base + count).AlignValue(0x10) - Base;

            return count;
        }

        public static void ReadImplements(ref byte[] data, uint EventsPosition, ushort NumImplements, ref T89RequiresImplementsSection section, T89ScriptObject script)
        {
            T89RequiresImplementsSection impl = new T89RequiresImplementsSection(script);
            section = impl;

            if (NumImplements < 1)
                return;

            if (data.Length <= EventsPosition)
                throw new ArgumentException("GSC could not be parsed because the requires_implements pointer was outside of boundaries of the input buffer.");

            BinaryReader reader = new BinaryReader(new MemoryStream(data));
            reader.BaseStream.Position = EventsPosition;

            for (byte i = 0; i < NumImplements; i++)
            {
                impl.Implements.Add(reader.ReadUInt64());
            }

            reader.Dispose();
        }

        public override byte[] Serialize()
        {
            byte[] data = new byte[Size()];

            BinaryWriter writer = new BinaryWriter(new MemoryStream(data));

            foreach (ulong s in Implements)
                writer.Write(s);

            writer.Dispose();

            return data;
        }

        public override void UpdateHeader(ref T89ScriptHeader Header)
        {
            Header.RequiresImplementsCount = (byte)Count();
            Header.RequiresImplementsTable = GetBaseAddress();

            Console.WriteLine($"{Header.RequiresImplementsTable:x}");
        }
    }
}