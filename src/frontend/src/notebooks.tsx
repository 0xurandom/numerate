import {
  BaseDirectory,
  exists,
  mkdir,
  readDir,
  readTextFile,
  writeTextFile,
} from "@tauri-apps/plugin-fs";

export async function initStorage(): Promise<void> {
  const folderExists: boolean = await exists("numerate", {
    baseDir: BaseDirectory.AppData,
  });

  if (!folderExists) {
    await mkdir("numerate", { baseDir: BaseDirectory.AppData });
  }
}

export interface Notebook {
  id: string;
  title: string;
  content: string;
  lastModified: number;
}

export async function saveNotebook(notebook: Notebook): Promise<void> {
  const fileName: string = `numerate/${notebook.id}.json`;
  const notebookJson: string = JSON.stringify(notebook);
  await writeTextFile(fileName, notebookJson, { baseDir: BaseDirectory.AppData });
}

export async function getNotebooks() : Promise<Notebook[]> {
  const notebooks: Array<Notebook> = [];
  const files = await readDir('numerate', { baseDir: BaseDirectory.AppData });

  for (const file of files) {
    if (file.isFile && file.name.endsWith('.json')) {
      const jsonStr = await readTextFile(`notebooks/${file.name}`, { baseDir: BaseDirectory.AppData });
      notebooks.push(JSON.parse(jsonStr));
    }
  }

  return notebooks;
}

export async function getNotebookByID(id: string): Promise<Notebook | null> {
  const targetName: string = `numerate/${id}.json`;
  const files = await readDir('numerate', { baseDir: BaseDirectory.AppData });

  for (const file of files) {
    if (file.isFile && file.name == targetName) {
      return JSON.parse(await readTextFile(`notebooks/${file.name}`, {baseDir: BaseDirectory.AppData}));
    }
  }
  return null;
}
