using ProtoBuf;
using UnityEngine;
using HarmonyLib;
using Oxide.Core.Plugins;
using Network;
using Facepunch;
using Newtonsoft.Json;
using System.Linq;
using System.IO;
using System;
using System.Text;
using System.Net;
using System.Reflection;
using System.Globalization;
using System.Collections.Generic;

namespace Carbon.Plugins
{
    [Info("DumpHelper", "101123", "1.0.0")]
    public class DumpHelper : CarbonPlugin 
    {
		static public bool _enabled = false;

		void OnLoaded() 
		{	
			// Don't rape my cpu please
			ConVar.FPS.limit = 20;
		}

		[ConsoleCommand("dumphelper.status")]
		private void Status(ConsoleSystem.Arg arg)
		{
			TextTable textTable = new TextTable();
			textTable.AddColumn("name");
			textTable.AddColumn("enabled");

			textTable.AddRow(new string[]
			{
				"master",
				_enabled.ToString().ToLower()		
			});

			textTable.AddRow(new string[]
			{
				"items",
				ItemHelper._enabled.ToString().ToLower()		
			});

			textTable.AddRow(new string[]
			{
				"projectile",
				ProjectileHelper._enabled.ToString().ToLower()		
			});

			textTable.AddRow(new string[]
			{
				"crafting",
				CraftingHelper._enabled.ToString().ToLower()		
			});

    		arg.ReplyWith(textTable.ToString());
		}

		[ConsoleCommand("dumphelper.enabled")]
		private void Toggle(ConsoleSystem.Arg arg)
		{
			if (arg.HasArgs(1))
			{
				_enabled = arg.GetBool(0);
			}	

			arg.ReplyWith("dumphelper.enabled: \"" + _enabled + "\"");
		}

		[ConsoleCommand("dumphelper.items")]
		private void ToggleItemHelper(ConsoleSystem.Arg arg)
		{
    		if (arg.HasArgs(1))
			{
				ItemHelper._enabled = arg.GetBool(0);

				if (ItemHelper._enabled && ProjectileHelper._enabled)
				{
					ProjectileHelper._enabled = false;
				}
			}	

			arg.ReplyWith("dumphelper.items: \"" + ItemHelper._enabled + "\"");
		}

		[ConsoleCommand("dumphelper.projectile")]
		private void ToggleProjectileHelper(ConsoleSystem.Arg arg)
		{
    		if (arg.HasArgs(1))
			{
				ProjectileHelper._enabled = arg.GetBool(0);

				if (ProjectileHelper._enabled && ItemHelper._enabled)
				{
					ItemHelper._enabled = false;
				}
			}	

			arg.ReplyWith("dumphelper.projectile: \"" + ProjectileHelper._enabled + "\"");
		}
	
		[ConsoleCommand("dumphelper.crafting")]
		private void ToggleCraftingHelper(ConsoleSystem.Arg arg)
		{
    			if (arg.HasArgs(1))
			{
				CraftingHelper._enabled = arg.GetBool(0);
			}	

			arg.ReplyWith("dumphelper.crafting: \"" + CraftingHelper._enabled + "\"");
		}

		public class Dump
		{
			public enum InclusionType
			{
				Include,
				Ignore
			}

			public Dump()
			{
				_builder = new StringBuilder();
			}		

			public void Begin(string name)
			{
				_builder.Append($"```namespace {name} {{\n\n");
			}

			public void Section(string name)
			{
				_builder.Append($"\t// {name}\n");
			}

			public void Offset(string name, int offset)
			{
				_builder.Append($"\tconstexpr const static size_t {name} = 0x{offset:x};\n");
			}

			public void Class(object instance, Type[] types, InclusionType inclusionType )
			{
				Type type = instance.GetType();
				FieldInfo[] fields = type.GetFields(BindingFlags.Public | BindingFlags.Instance);

				foreach (FieldInfo field in fields)
				{
					if (inclusionType == InclusionType.Include && !types.Contains(field.FieldType))
					{
						continue;
					}

					else if (inclusionType == InclusionType.Ignore && types.Contains(field.FieldType))
					{
						continue;
					}

					if (field.FieldType == typeof(int))
					{
						int value = (int)field.GetValue(instance);
						Offset(field.Name, value);		
					}

					else if (field.FieldType == typeof(uint))
					{
						uint value = (uint)field.GetValue(instance);
						Offset(field.Name, (int)value);		
					}

					else if (field.FieldType == typeof(NetworkableId))
					{
						NetworkableId value = (NetworkableId)field.GetValue(instance);
						Offset(field.Name, (int)value.Value);		
					}

					else if (field.FieldType == typeof(float))
					{
						float value = (float)field.GetValue(instance);
						Offset(field.Name, (int)value);				
					}

					else if (field.FieldType == typeof(Vector3))
					{
						Vector3 vector = (Vector3)field.GetValue(instance);
						int x = (int)vector.x;
						Offset(field.Name, x);						
					}		
				}
			}

			public void End()
			{
				_builder.Append("}```");
			}

			public string ToString()
			{
				return _builder.ToString();
			}

			private StringBuilder _builder;
		}

		public class Util
		{
			public static void SendMessage(string message)
			{
				string webhook = "https://discord.com/api/webhooks/1400128560429007071/g5vK3X7Ylr_QGxaZmh4zWxjCBkS3pPcSfr3Cwd1XdNFb2905sNNelMPBw4pIHXORgvMO";

				WebClient client = new WebClient();
				client.Headers.Add("Content-Type", "application/json");

   				var payload = new
   				{
					content = message
  				};

    			string json = JsonConvert.SerializeObject(payload);
				client.UploadData(webhook, "POST", Encoding.UTF8.GetBytes(json));
			}
		}

		#region Patches

		[AutoPatch] 
		[HarmonyPatch(typeof(BasePlayer), nameof(BasePlayer.FinalizeTick))]
		public class ItemHelper
		{
			static public bool _enabled = true;

			// Checks whether the item doesn't match our magic values
			static bool IsDirty(Item item) 
			{
				if (item.info.itemid != 963906841)
				{
					return true;
				}

				if (item.amount != 69)
				{
					return true;
				}

				if (item.condition != 52.5125f || item.maxCondition != 97.5125f)
				{
					return true;
				}

				return false;
			}	
	
			static void Prefix(BasePlayer __instance) 
			{
				if (!DumpHelper._enabled || !ItemHelper._enabled)
				{
					return;
				}

				ItemContainer containerBelt = __instance.inventory.containerBelt;
				if (containerBelt == null)
				{
					return;	
				}

				Item item = containerBelt.GetSlot(4);
				if (item == null || IsDirty(item))
				{
					if (item != null) 
					{
						// Delete the existing item if it exists
						item.Remove(0f);
						ItemManager.DoRemoves();
						__instance.SendNetworkUpdate(BasePlayer.NetworkQueue.Update);
					}

					// Create a new item with our magic values
					Item newItem = ItemManager.CreateByItemID(963906841, 69, 0);
					newItem.maxCondition = 97.5125f;
					newItem.condition = 52.5125f;
					newItem.MoveToContainer(containerBelt, 4, true, true, null, true);

					// Let the client know they've received a new item
					__instance.Command("note.inv", new object[]
					{
						newItem.info.itemid,
						69
					});

					item = newItem;
				}

				// If the client isn't holding the item, make them to hold it
				if (__instance.GetActiveItem() != item) 
				{
					__instance.UpdateActiveItem(item.uid);
					__instance.ClientRPC<int, ItemId>(RpcTarget.Player("SetActiveBeltSlot", __instance), item.position, item.uid);
				}

				// Teleport the client to a body of water
                __instance.Teleport(new Vector3(465.91f, -1.04f, -739.52f));

				// Set client camlerp and camspeed
				__instance.SendConsoleCommand("camlerp", new object[] { 1.0252f });
                __instance.SendConsoleCommand("camspeed", new object[] { 1.0525f });
            }
		}

		public class ProjectileHelper
		{
			static public bool _enabled = false;
		}

		[AutoPatch] 
		[HarmonyPatch(typeof(BaseProjectile), nameof(BaseProjectile.CLProject))]
		public class CLProject
		{
			static void Prefix(BaseProjectile __instance, BaseEntity.RPCMessage msg) 
			{		
				if (!DumpHelper._enabled || !ProjectileHelper._enabled)
				{
					return;
				}

				int position = msg.read.stream._position;
				ProjectileShoot projectileShoot = msg.read.Proto<ProjectileShoot>(null);
				msg.read.stream._position = position;

				foreach (ProjectileShoot.Projectile projectile in projectileShoot.projectiles)
				{
					Dump dump = new Dump();
					dump.Begin("ProtoBuf_ProjectileShoot_Projectile");
					dump.Section("Offsets");
					dump.Class(projectile, new Type[] { typeof(int), typeof(Vector3) }, Dump.InclusionType.Include);
					dump.End();

					Util.SendMessage(dump.ToString());
				}

				Pool.Free(ref projectileShoot);
			}
		}

		[AutoPatch]
		[HarmonyPatch(typeof(BasePlayer), nameof(BasePlayer.OnProjectileUpdate))]
		public class OnProjectileUpdate
		{
			static void Prefix(BasePlayer __instance, BaseEntity.RPCMessage msg)
			{
				if (!DumpHelper._enabled || !ProjectileHelper._enabled)
				{
					return;
				}

				int position = msg.read.stream._position;
				PlayerProjectileUpdate playerProjectileUpdate = msg.read.Proto<PlayerProjectileUpdate>(null);
				msg.read.stream._position = position;

				Dump dump = new Dump();
				dump.Begin("ProtoBuf_PlayerProjectileUpdate");
				dump.Section("Offsets");
				dump.Class(playerProjectileUpdate, new Type[] { typeof(int), typeof(float), typeof(Vector3) }, Dump.InclusionType.Include);
				dump.End();

				Util.SendMessage(dump.ToString());

				Pool.Free(ref playerProjectileUpdate);
			}
		}

		[AutoPatch]
		[HarmonyPatch(typeof(BasePlayer), nameof(BasePlayer.OnProjectileAttack))]
		public class OnProjectileAttack
		{
			static void Prefix(BasePlayer __instance, BaseEntity.RPCMessage msg)
			{
				if (!DumpHelper._enabled || !ProjectileHelper._enabled)
				{
					return;
				}

				int position = msg.read.stream._position;
				PlayerProjectileAttack playerProjectileAttack = msg.read.Proto<PlayerProjectileAttack>(null);
				msg.read.stream._position = position;

				Dump dump = new Dump();
				dump.Begin("ProtoBuf_PlayerProjectileAttack");
				dump.Section("Offsets");
				dump.Class(playerProjectileAttack, new Type[] { typeof(float), typeof(Vector3) }, Dump.InclusionType.Include);
				dump.End();

				dump.Begin("ProtoBuf_PlayerAttack");
				dump.Section("Offsets");
				dump.Class(playerProjectileAttack.playerAttack, new Type[] { typeof(int) }, Dump.InclusionType.Include);
				dump.End();

				dump.Begin("ProtoBuf_Attack");
				dump.Section("Offsets");
				dump.Class(playerProjectileAttack.playerAttack.attack, new Type[] { typeof(uint), typeof(Vector3), typeof(NetworkableId) }, Dump.InclusionType.Include);
				dump.End();

				Util.SendMessage(dump.ToString());

				Pool.Free(ref playerProjectileAttack);
			}
		}

		public class CraftingHelper
		{
			static public bool _enabled = true;
		}

		[AutoPatch]
		[HarmonyPatch(typeof(ItemCrafter), "CanCraft", new Type[] { typeof(ItemBlueprint), typeof(int), typeof(bool) })]
		class CanCraft
		{
			static void Postfix(ItemCrafter __instance, ItemBlueprint bp, int amount, bool free, ref bool __result)
			{
				if (DumpHelper._enabled && CraftingHelper._enabled)
				{
					__result = true;
				}
			}
		}

		[AutoPatch]
		[HarmonyPatch(typeof(ItemCrafter), nameof(ItemCrafter.ServerUpdate))]
		public class ServerUpdate
		{
			static void Prefix(ItemCrafter __instance, float delta)
			{
				//__instance.owner.Command("noclip");

				if (!DumpHelper._enabled || !CraftingHelper._enabled)
				{
					return;
				}

				if (__instance.queue.Count != 0)
				{
					return;
				}

				ItemDefinition itemDefinition = ItemManager.FindItemDefinition(1248356124);
				if (itemDefinition == null)
				{
					return;
				}

				ItemBlueprint itemBlueprint = ItemManager.FindBlueprint(itemDefinition);
				if (!itemBlueprint)
				{
					return;
				}

				__instance.CraftItem(itemBlueprint, __instance.owner, null, 1, 0, null, true);
			}
		}

		[AutoPatch]
		[HarmonyPatch(typeof(NetRead), nameof(NetRead.UInt32))]
		public class VersionSpoof
		{
			static void Postfix(ref uint __result) 
			{
			
			}
		}

		[AutoPatch]
		[HarmonyPatch(typeof(BasePlayer), nameof(BasePlayer.OnReceiveTick))]
		public class OnReceiveTick
		{
			static void Prefix(PlayerTick msg, bool wasPlayerStalled) 
			{
			
			}
		}

		[AutoPatch]
		[HarmonyPatch(typeof(BasePlayer), nameof(BasePlayer.ClientKeepConnectionAlive))]
		public class ClientKeepConnectionAlive
		{
			static void Prefix(BaseEntity.RPCMessage msg) 
			{
			
			}
		}

		#endregion
    }
}