﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Ponykart.Networking {

    enum PacketSource {
        Remote,
        Local
    }

    public struct Message {
        Commands Type;
        byte[] Contents;
        bool Volatile;
        public PonykartPacket ToPKPacket(Connection owner) {
            return new PonykartPacket(Type, Contents, owner, Volatile);
        }
        public Message(Commands type, byte[] contents, bool isVolatile) {
            Type = type;
            Contents = contents;
            Volatile = isVolatile;
        }
        public Message(Commands type, string contents, bool isVolatile) {
            Type = type;
            Contents = System.Text.ASCIIEncoding.ASCII.GetBytes(contents);
            Volatile = isVolatile;
        }
    }

    public class PonykartPacket {
        Int64 Timestamp;
        Int16 _Type;
        Connection Owner;
        PacketSource Source;
        public bool Volatile = false;
        public Int64 UnifiedTime {
            get {
                if (Source == PacketSource.Local) {
                    return (Timestamp - Owner.ZeroMoment);
                } else {
                    return (Timestamp - Owner.RemoteOffset);
                }
            }
        }
        public Commands Type {
            get {
                return Enum.IsDefined(typeof(Commands), _Type) ? (Commands)Type : Commands.NoMessage;
            }
        }
        byte[] Contents;
        public string StringContents {
            get {
                return System.Text.ASCIIEncoding.ASCII.GetString(Contents);
            }
        }
        /// <summary>
        /// Get specific header information from a ponykart packet
        /// Packet format: TTTTTTTTYYCC....
        ///                0       8 10
        /// Timestamp, Type, Contents
        /// </summary>
        static Int64 GetTimestamp(byte[] creator) {
            return BitConverter.ToInt64(creator, 0);
        }
        static Int16 GetPType(byte[] creator) {
            return BitConverter.ToInt16(creator, 8);
        }
        /// <summary>
        /// Strips out header information from a packet and returns the contents
        /// </summary>
        static byte[] GetContents(byte[] packet) {
            var contents = new byte[packet.Length - 10];
            Array.Copy(packet, 10, contents, 0, packet.Length - 10);
            return contents;
        }

        /// <summary>
        /// Creates a packet byte array.
        /// </summary>
        public byte[] ToBytes() {
            var timeArr = BitConverter.GetBytes(Timestamp);
            var typeArr = BitConverter.GetBytes(_Type);
            var packet = new byte[Contents.Length + 10];
            Array.Copy(timeArr, 0, packet, 0, 8);
            Array.Copy(typeArr, 0, packet, 8, 2);
            Array.Copy(Contents, 0, packet, 10, Contents.Length);
            return packet;
        }

        public void SetOrigin(Connection receiver){
            Owner = receiver;
        }

        public PonykartPacket(byte[] creator) { // Incoming packe
            Contents = GetContents(creator);
            Timestamp = GetTimestamp(creator);
            _Type = GetPType(creator);
            Source = PacketSource.Remote;
        }

        public PonykartPacket(Commands type, byte[] contents, Connection c, bool isVolatile) { // Outgoing
            Contents = contents;
            Timestamp = System.DateTime.Now.Ticks;
            _Type = (Int16)type;
            Owner = c;
            Source = PacketSource.Local;
            Volatile = isVolatile;
        }

        public PonykartPacket(Commands type, string contents, Connection c, bool isVolatile) { // Outgoing
            Contents = System.Text.ASCIIEncoding.ASCII.GetBytes(contents);
            Timestamp = System.DateTime.Now.Ticks;
            _Type = (Int16)type;
            Owner = c;
            Source = PacketSource.Local;
            Volatile = isVolatile;
        }
    }
}
