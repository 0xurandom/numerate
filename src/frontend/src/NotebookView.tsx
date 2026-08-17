import { ScrollArea } from "./components/ui/scroll-area";
import { FileText } from "lucide-react";
import { Card, CardHeader, CardTitle } from "./components/ui/card";
import { initStorage, Notebook, getNotebooks } from "./notebooks";

export async function NotebookView() {
  initStorage();
  const notebooks: Notebook[] = await getNotebooks();

  return (
    <div>
      <ScrollArea>
        {notebooks.length == 0 ? (
          <div>
            <FileText className="text-muted-foreground/50">
              <h3>No notebooks found</h3>
            </FileText>
          </div>
        ) : (
          <div>
            {notebooks.map((notebook) => (
              <Card key={notebook.id} onClick={() => null}>
                <CardHeader>
                  <CardTitle>
                    {notebook.title}
                  </CardTitle>
                </CardHeader>
              </Card>
            ))}
          </div>
        )}
      </ScrollArea>
    </div>
  );
}
