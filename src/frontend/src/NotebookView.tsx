import { ScrollArea } from "./components/ui/scroll-area";
import { FileText, Clock, Plus } from "lucide-react";
import { Card, CardContent, CardFooter, CardHeader, CardTitle } from "./components/ui/card";
import { initStorage, Notebook, getNotebooks } from "./notebooks";
import { useState, useEffect } from "react";

function getDate(timestamp: number): string  {
  return new Intl.DateTimeFormat('en-GB', {
    month: 'short',
    day: 'numeric',
    hour: 'numeric',
    minute: 'numeric'
  }).format(new Date(timestamp))
}


export function NotebookView() {
  const [notebooks, setNotebooks] = useState<Notebook[]>([]);

  useEffect(() => {
    initStorage();
    getNotebooks().then(setNotebooks);
  }, []);

  return (
    <div>
      <ScrollArea>
        {notebooks.length == 0 ? (
          <div className="flex flex-col items-center gap-4">
            <NewNoteBookCard onClick={() => { null }} />
            <p className="text-muted-foreground text-sm">No Notebooks yet; Create your first</p>
          </div>
        ) : (
          <div className="grid grid-cols-1 gap-6 pb-8">
            {notebooks.map((notebook) => (
              <Card key={notebook.id} onClick={() => null}>
                <CardHeader className="pb-8">
                  <div>
                    <CardTitle className="text-xl">
                      {notebook.title}
                    </CardTitle>
                  </div>
                  <div>
                    <Clock />
                    {getDate(notebook.lastModified)}
                  </div>
                </CardHeader>
                <CardFooter className="pt-0 mt-auto">
                  <div className="absolute inset-0 bg-muted/30 rounded-md overflow-hidden text-xs text-muted font-mono">
                    {(notebook.content == '') ? 'Empty notebook...' : notebook.content}
                  </div>
                </CardFooter>
              </Card>
            ))}
          </div>
        )}
      </ScrollArea>
    </div>
  );
}

interface NewNotebookCardProps {
  onClick: () => void;
}

export function NewNoteBookCard({onClick } : NewNotebookCardProps) {
  return (
    <Card onClick = {onClick} className="flex items-center justify-center cursor-pointer hover:border-foreground/30 hover:bg-muted/30 ">
      <Plus size={30} />
      <span className="text-sm font-medium">New notebook</span>
    </Card>
  );
}
