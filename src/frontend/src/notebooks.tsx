import {
  BaseDirectory,
  exists,
  mkdir,
  readTextFile,
  writeTextFile,
} from "@tauri-apps/plugin-fs";

async function initStorage() {
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

function saveNotebook(notebook: Notebook) {
  const fileName: string = `numerate/${notebook.id}.json`;
  const notebookJson: string = JSON.stringify(notebook);
  writeTextFile(fileName, notebookJson, { baseDir: BaseDirectory.AppData });
}

function getNotebookByID(id: string): string {
  const fileName: `numerate/${id}.json`;
  const notebookJson: Promise<string> = readTextFile(fileName, );
}
