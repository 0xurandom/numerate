import {
  Menubar,
  MenubarContent,
  MenubarGroup,
  MenubarItem,
  MenubarMenu,
  MenubarShortcut,
  MenubarTrigger,
} from "@/components/ui/menubar";

import { FileIcon } from "lucide-react";

export default function MenuBar() {
  return (
    <Menubar className="border-b rounded-none px-2 lg:px-4">

      <MenubarMenu>
        <MenubarContent>
          <MenubarTrigger className="font-medium">
          <FileIcon />
          File
        </MenubarTrigger>
          <MenubarGroup>
            <MenubarItem>
              Open <MenubarShortcut>Ctrl + O</MenubarShortcut>
            </MenubarItem>
          </MenubarGroup>
        </MenubarContent>
      </MenubarMenu>

      <MenubarMenu>
        <MenubarTrigger>Edit</MenubarTrigger>
        <MenubarContent>
          <MenubarGroup>
            <MenubarItem>
              test item <MenubarShortcut>command + no</MenubarShortcut>
            </MenubarItem>
          </MenubarGroup>
        </MenubarContent>
      </MenubarMenu>
    </Menubar>
  );
}
