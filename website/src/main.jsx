import React from 'react'
import ReactDOM from 'react-dom/client'
import {
  createBrowserRouter,
  RouterProvider,
  Route,
} from "react-router-dom";

import Root from './root';
import ErrorPage from './error-page';
import AllLotes from './pages/AllLotes';
import LoteDetails from './pages/LoteDetails';
import ReceitaDetails from './pages/ReceitaDetails';

import "./index.css"

const router = createBrowserRouter([
  {
    path: "/",
    element: <Root />,
    errorElement: <ErrorPage />,
    children: [
      {
        path: "/",
        element: <AllLotes />
      },
      {
        path: "/historico/lote/:id",
        element: <LoteDetails />
      },
      {
        path: "/historico/receita/:id",
        element: <ReceitaDetails />
      }
    ]
  },
]);

ReactDOM.createRoot(document.getElementById('root')).render(
  <>
    <RouterProvider router={router} />
  </>
)
